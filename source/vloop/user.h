#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "afe.h"
#include "afe_rst.h"
#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
#include "pops_current_balancing.h"
#include "pops_current_balancing_old.h"
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
              interrupt_2("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              interrupt_1("timer", *this, std::chrono::microseconds(100), RTTask),
              afe("afe_rst", *this),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_2;
        vslib::TimerInterrupt<Converter>      interrupt_1;
        ActiveFrontEndRST                     afe;
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
            interrupt_2.stop();
        }

        constexpr static int n_elements = 10'003;

        void backgroundTask() override
        {
#ifdef PERFORMANCE_TESTS
            if (counter > n_elements)
            {
                interrupt_1.stop();
                interrupt_2.stop();
                double const mean = interrupt_1.average() / 1.2;
                std::cout << "Average time per interrupt: (" << mean << " +- "
                          << interrupt_1.standardDeviation(mean) / 1.2 << ") ns" << std::endl;
                auto const histogram = interrupt_1.histogramMeasurements<100>(interrupt_1.min(), interrupt_1.max());
                for (auto const& value : histogram.getData())
                {
                    std::cout << value << " ";
                }
                std::cout << std::endl;
                auto const bin_with_max = histogram.getBinWithMax();
                auto const edges        = histogram.getBinEdges(bin_with_max);
                std::cout << "bin with max: " << bin_with_max
                          << ", centered at: " << 0.5 * (edges.first / 1.2 + edges.second / 1.2) << std::endl;
                const auto min = interrupt_1.min() / 1.2;
                const auto max = interrupt_1.max() / 1.2;
                std::cout << "min: " << min << " ns, max: " << max << " ns" << std::endl;
                exit(0);
            }
#endif
        }

        template<typename SourceType, typename TargetType>
        static TargetType cast(SourceType input)
        {
            return std::bit_cast<TargetType>(input);
        }

        static void RTTask(Converter& converter)
        {
            // collect inputs
            for (std::size_t i = 0; i < num_data; ++i)
            {
                converter.m_data_1[i] = cast<uint64_t, double>(converter.m_s2r->data[i].value);
                converter.m_data[i]   = (std::rand() - 1.0) * 100.0 / RAND_MAX;
            }

            const double regulation_on = converter.m_data[0];
            const double v_dc_ref      = converter.m_data[1];
            const double v_dc_meas     = converter.m_data[2];
            const double q_ref         = converter.m_data[3];
            const double v_a           = converter.m_data[4];
            const double v_b           = converter.m_data[5];
            const double v_c           = converter.m_data[6];
            const double i_a           = converter.m_data[7];
            const double i_b           = converter.m_data[8];
            const double i_c           = converter.m_data[9];

            const auto [v_a_ref, v_b_ref, v_c_ref]
                = converter.afe.vdc_control(v_a, v_b, v_c, i_a, i_b, i_c, v_dc_ref, v_dc_meas, q_ref, regulation_on);

            converter.m_data[0] = v_a_ref;
            converter.m_data[1] = v_b_ref;
            converter.m_data[2] = v_c_ref;

            // write to output registers
            for (uint32_t index = 0; index < num_data; index++)
            {
                converter.m_r2s->data[index].value = cast<double, uint64_t>(converter.m_data_1[index]);
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

        int counter{0};

        constexpr static uint32_t    num_data{20};
        std::array<double, num_data> m_data;
        std::array<double, num_data> m_data_1;

        volatile stream_to_reg* m_s2r;
        volatile reg_to_stream* m_r2s;
    };

}   // namespace user
