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
              interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              afe("afe_rst", *this),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
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
        }

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

        int counter = 0;

        static void RTTask(Converter& converter)
        {
            constexpr static uint32_t         num_data      = 40;
            constexpr static uint32_t         num_data_half = 20;
            std::array<double, num_data_half> data_in;

            // collect inputs
            for (std::size_t i = 0; i < num_data_half; ++i)
            {
                data_in[i] = cast<uint64_t, double>(converter.m_s2r->data[i].value);
            }

            const double regulation_on = data_in[0];
            const double v_dc_ref      = data_in[1];
            const double v_dc_meas     = data_in[2];
            const double q_ref         = data_in[3];
            const double v_a           = data_in[4];
            const double v_b           = data_in[5];
            const double v_c           = data_in[6];
            const double i_a           = data_in[7];
            const double i_b           = data_in[8];
            const double i_c           = data_in[9];

            // zero outputs to avoid confusion
            for (int index = 0; index < num_data_half; index++)
            {
                data_in[index] = 0.0;   // zeroing as this channel is reused for output
            }

            // perform active front-end control
            const auto [v_a_ref, v_b_ref, v_c_ref]
                = converter.afe.vdc_control(v_a, v_b, v_c, i_a, i_b, i_c, v_dc_ref, v_dc_meas, q_ref, regulation_on);

            // set outputs:
            data_in[0] = v_a_ref;
            data_in[1] = v_b_ref;
            data_in[2] = v_c_ref;

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

            // if(regulation_on>0 && converter.counter++ > 100)
            // {
            //     converter.interrupt_1.stop();
            // }
        }

      private:
        int m_interrupt_id;

        volatile stream_to_reg* m_s2r;
        volatile reg_to_stream* m_r2s;
    };

}   // namespace user
