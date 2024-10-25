#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
#include "pops_dispatcher.h"
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
              control_period(*this, "control_period", 0.0),
              dispatcher(*this),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
        POPSDispatcher                        dispatcher;

        // ...
        // end of your Components

        // Define your Parameters here
        vslib::Parameter<double> control_period;
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
        constexpr static int n_elements     = 10'000;

        void backgroundTask() override
        {
#ifdef PERFORMANCE_TESTS
            if (counter > n_elements)
            {
                std::cout << "counter: " << counter << " " << n_elements << " " << (counter > n_elements) << std::endl;
                interrupt_1.stop();
                double const mean = interrupt_1.average();
                std::cout << "Average time per interrupt: " << mean << " +- " << interrupt_1.standardDeviation(mean)
                          << std::endl;
                // auto const histogram = interrupt_1.histogramMeasurements<100>(time_range_min,
                // time_range_max); for (auto const& value : histogram.getData())
                //             {
                //                 std::cout << value << " ";
                //             }
                //             std::cout << std::endl;
                //             auto const bin_with_max = histogram.getBinWithMax();
                //             auto const edges        = histogram.getBinEdges(bin_with_max);
                //             std::cout << "bin with max: " << bin_with_max
                //                       << ", centered at: " << 0.5 * (edges.first + edges.second) << std::endl;
                const auto min = interrupt_1.min();
                const auto max = interrupt_1.max();
                std::cout << "min: " << min << ", max: " << max << std::endl;
                exit(0);
            }
#endif
        }

        template<typename SourceType, typename TargetType>
        static TargetType cast(SourceType input)
        {
            return *reinterpret_cast<TargetType*>(&input);
        }


        unsigned long interrupt_counter{0};
        double        previous_cyclic_data{-1};

        static void RTTask(Converter& converter)
        {
            constexpr uint32_t                num_data      = 40;
            constexpr uint32_t                num_data_half = 20;
            std::array<double, num_data_half> data_in;

            for (std::size_t i = 0; i < num_data_half; ++i)
            {
                data_in[i] = cast<uint64_t, double>(converter.m_s2r->data[i].value);
            }

            const double cyclic_data_input = data_in[0];
            for (int index = 0; index < converter.dispatcher.TotalNumberDCDC; index++)
            {
                converter.dispatcher.setVdcMeas(index, data_in[2 + index]);
            }

            const double v_ref  = data_in[8];
            const int    n_dcdc = data_in[9];
            const double i_meas = data_in[10];

            // calculate and set new values

            for (int index = 0; index < num_data_half; index++)
            {
                data_in[index] = 0.0;   // zeroing as this channel is reused for output
            }
            data_in[0] = cyclic_data_input;

            // detect new cycle: c0=1, c_tim arbitrary
            if (cyclic_data_input > -1 && converter.previous_cyclic_data < 0)
            {
                converter.c_tim = 0;   // time = 0, new cycle begins
            }

            if (converter.c_tim < 30)
            {
                converter.dispatcher.setCyclicData(converter.c_tim, cyclic_data_input);
            }

            const double current_time     = converter.c_tim * converter.control_period;   // inside a cycle
            double       i_meas_estimated = 0.0;
            if (converter.c_tim > 4)   // c_tim > 4, we can start outputting reference
            {
                i_meas_estimated = converter.dispatcher.getReference(current_time);
            }
            data_in[2] = i_meas_estimated;

            if (converter.c_tim == 30)
            {
                converter.dispatcher.printCyclicData();
                converter.dispatcher.endTimeLastPlateau();
            }

            if (converter.c_tim >= 30)
            {
                converter.dispatcher.dispatchVoltage(current_time, v_ref, i_meas_estimated);
                for (int index = 0; index < converter.dispatcher.getModulationId().size(); index++)
                {
                    std::cout << converter.dispatcher.getModulationId()[index] << std::endl;
                    data_in[index + 3] = converter.dispatcher.getModulationId()[index];
                }
            }

            data_in[11] = converter.dispatcher.getNdcdc(current_time);

            // message received, update c_tim
            converter.c_tim++;
            converter.interrupt_counter++;
            converter.previous_cyclic_data = cyclic_data_input;

            // write to output registers
            for (uint32_t index = 0; index < num_data_half; index++)
            {
                converter.m_r2s->data[index].value = cast<double, uint64_t>(data_in[index]);
            }

            // send it away
            // kria transfer rate: 100us
            converter.m_r2s->num_data = num_data;
            converter.m_r2s->tkeep    = 0x0000FFFF;

            // trigger connection
            converter.m_r2s->ctrl = REG_TO_STREAM_CTRL_START;
            converter.counter++;
        }

      private:
        int m_interrupt_id;

        volatile stream_to_reg* m_s2r;
        volatile reg_to_stream* m_r2s;

        unsigned int c_tim{0};
    };

}   // namespace user
