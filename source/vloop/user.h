#pragma once

#include <unistd.h>

#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
#include "vslib.h"

namespace user
{

    class Converter : public vslib::IConverter
    {
      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              m_interrupt_id{121 + 0},   // Jonas's definition
              interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              pid("pid_1", *this),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
        vslib::PID                            pid;

        // ...
        // end of your Components

        // Define your Parameters here

        // end of your Parameters

        void init() override
        {
            if (m_s2r->ctrl & STREAM_TO_REG_CTRL_PMA_INIT)
            {
                m_s2r->ctrl &= ~STREAM_TO_REG_CTRL_PMA_INIT;
            }
            sleep(2);
            if (m_s2r->ctrl & STREAM_TO_REG_CTRL_RESET_PB)
            {
                m_s2r->ctrl &= ~STREAM_TO_REG_CTRL_RESET_PB;
            }
            sleep(1);

            m_s2r->ctrl |= STREAM_TO_REG_CTRL_SEL_OUTPUT;

            if (!(m_s2r->status
                  & (STREAM_TO_REG_STATUS_CHANNEL_UP | STREAM_TO_REG_STATUS_GT_PLL_LOCK | STREAM_TO_REG_STATUS_LANE_UP
                     | STREAM_TO_REG_STATUS_PLL_LOCKED | STREAM_TO_REG_STATUS_GT_POWERGOOD)))
            {
                printf("Unexpected status: 0x%#08x\n", m_s2r->ctrl);
            }

            if (m_s2r->status & (STREAM_TO_REG_STATUS_LINK_RESET | STREAM_TO_REG_STATUS_SYS_RESET))
            {
                printf("Link is in reset\n");
            }

            if (m_s2r->status & (STREAM_TO_REG_STATUS_SOFT_ERR | STREAM_TO_REG_STATUS_HARD_ERR))
            {
                printf("Got an error\n");
            }

            printf("Link up and good. Ready to receive data.\n");
            interrupt_1.start();
        }

        int                  counter        = 0;
        int                  expected_delay = 210;
        int                  time_range_min = expected_delay - 20;   // in clock ticks
        int                  time_range_max = expected_delay + 20;   // in clock ticks
        constexpr static int n_elements     = 1000;

        void backgroundTask() override
        {
            // reset the PID every 2 minutes
            if(!m_recently_used)
            {
                sleep(120);
                std::cout << "resetting pid\n";
                pid.reset();
            }
            m_recently_used = false;
            //             while (true)
            //             {
            //                 if (counter == n_elements + 50)
            //                 {
            //                     interrupt_1.stop();
            // #ifdef PERFORMANCE_TESTS
            //                     double const mean = interrupt_1.average();
            //                     std::cout << "Average time per interrupt: " << mean << " +- " <<
            //                     interrupt_1.standardDeviation(mean)
            //                               << std::endl;
            //                     auto const histogram = interrupt_1.histogramMeasurements<100>(time_range_min,
            //                     time_range_max); for (auto const& value : histogram.getData())
            //                     {
            //                         std::cout << value << " ";
            //                     }
            //                     std::cout << std::endl;
            //                     auto const bin_with_max = histogram.getBinWithMax();
            //                     auto const edges        = histogram.getBinEdges(bin_with_max);
            //                     std::cout << "bin with max: " << bin_with_max
            //                               << ", centered at: " << 0.5 * (edges.first + edges.second) << std::endl;
            // #endif
            //                     break;
            //                 }
            //                 __asm volatile("wfi");
            //                 counter++;
            //             }
        }

        template<typename SourceType, typename TargetType>
        static TargetType cast(SourceType input)
        {
            return *reinterpret_cast<TargetType*>(&input);
        }

        static double getDouble(const uint32_t low, const uint32_t high)
        {
            const uint64_t    uint64_value   = ((uint64_t)high) << 32 | low;
            return cast<uint64_t, double>(uint64_value);
        }

        static std::tuple<uint32_t, uint32_t> splitDouble(const double input)
        {
            const uint64_t output  = cast<double, uint64_t>(input);
            const uint32_t high      = (uint32_t)(output >> 32);
            const uint32_t low       = (uint32_t)output;
            return std::make_tuple(low, high);
        }

        static double readDouble(volatile stream_to_reg* s2r, const uint32_t speedgoat_index)
        {
            volatile uint32_t low    = s2r->data[2 * speedgoat_index].value;
            volatile uint32_t high   = s2r->data[2 * speedgoat_index + 1].value;
            return getDouble(low, high);
        }

        static void writeDouble(const double value, volatile reg_to_stream* r2s, const uint32_t speedgoat_index)
        {
            auto [out32_low, out32_high] = splitDouble(value);
            r2s->data[2 * speedgoat_index].value     = out32_low;
            r2s->data[2 * speedgoat_index + 1].value = out32_high;
        }

        static void RTTask(Converter& converter)
        {
            // back to TEST1: read data and send it back
            // for (uint32_t i = 0; i < (converter.m_s2r->num_data - 1); i += 2)
            // {
            //     double output = readDouble(converter.m_s2r, i);
            //     output        *= 2.0;
            //     writeDouble(output, converter.m_r2s, i);
            // }

            // TEST 4: Control simple system using PID with double-precision variables
            const auto reference = readDouble(converter.m_s2r, 0);
            const auto measurement = readDouble(converter.m_s2r, 1);

            // use the numbers
            const auto actuation = converter.pid.control(measurement, reference);

            // write them to stream
            writeDouble(actuation, converter.m_r2s, 0);

            // write everything else
            for (uint32_t i = 2; i < converter.m_s2r->num_data; i++)
            {
                converter.m_r2s->data[i].value = converter.m_s2r->data[i].value;
            }

            // send it away

            // kria transfer rate: 100us
            converter.m_r2s->num_data = converter.m_s2r->num_data;
            converter.m_r2s->tkeep    = converter.m_s2r->keep[converter.m_s2r->num_data - 1].value;

            // trigger connection
            converter.m_r2s->ctrl |= REG_TO_STREAM_CTRL_START;
            converter.m_recently_used = true;
        }

        bool m_recently_used{false};

      private:
        int m_interrupt_id;

        volatile stream_to_reg* m_s2r;
        volatile reg_to_stream* m_r2s;
    };

}   // namespace user
