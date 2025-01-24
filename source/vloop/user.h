#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "afe.h"
#include "afe_rst.h"
#include "afe_vdc_bal.h"
#include "cheby_gen/reg_to_stream_cpp.h"
#include "cheby_gen/stream_to_reg_cpp.h"
#include "halfBridge.h"
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
              interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              afe_vdc_bal("afe_rst", *this),
              rst_vdc("rst_vdc", *this),
              iir_vdc("iir_vdc", *this),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
        ActiveFrontEndVdcBalance              afe_vdc_bal;
        vslib::RST<1>                         rst_vdc;
        vslib::IIRFilter<2>                   iir_vdc;
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

        constexpr static int n_elements = 100'000;

        void backgroundTask() override
        {
#ifdef PERFORMANCE_TESTS
            if (counter > n_elements)
            {
                interrupt_1.stop();
                const double clk_freq = 1.33333;   // in ns
                const double mean     = interrupt_1.average() / clk_freq;
                std::cout << "Average time per interrupt: (" << mean << " +- "
                          << interrupt_1.standardDeviation(interrupt_1.average()) / clk_freq << ") ns" << std::endl;
                const auto histogram = interrupt_1.histogramMeasurements<100>(interrupt_1.min(), interrupt_1.max());
                for (auto const& value : histogram.getData())
                {
                    std::cout << value << " ";
                }
                std::cout << std::endl;
                const auto bin_with_max = histogram.getBinWithMax();
                const auto edges        = histogram.getBinEdges(bin_with_max);
                std::cout << "bin with max: " << bin_with_max
                          << ", centered at: " << 0.5 * (edges.first / clk_freq + edges.second / clk_freq) << std::endl;
                const auto min = interrupt_1.min() / clk_freq;
                const auto max = interrupt_1.max() / clk_freq;
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
            for (uint32_t index = 0; index < num_data; index++)
            {
                converter.m_data[index] = cast<uint64_t, double>(converter.m_s2rcpp.data[index].read());
            }

            const double regulation_on = converter.m_data[0];
            const double v_dc_ref      = converter.m_data[1];
            const double v_dc_p        = converter.m_data[2];
            const double v_dc_n        = converter.m_data[3];
            const double q_ref         = converter.m_data[4];
            const double v_a           = converter.m_data[5];
            const double v_b           = converter.m_data[6];
            const double v_c           = converter.m_data[7];
            const double i_a           = converter.m_data[8];
            const double i_b           = converter.m_data[9];
            const double i_c           = converter.m_data[10];

            const double v_dc_meas = v_dc_p + v_dc_n;
            const double v_dc_diff = v_dc_p - v_dc_n;

            const auto [v_a_ref, v_b_ref, v_c_ref] = converter.afe_vdc_bal.vdc_control(
                v_a, v_b, v_c, i_a, i_b, i_c, v_dc_ref, v_dc_meas, q_ref, regulation_on
            );

            const auto v_dc_diff_filtered = converter.iir_vdc.filter(regulation_on * v_dc_diff);
            const auto m0                 = converter.rst_vdc.control(0.0, regulation_on * v_dc_diff_filtered);

            converter.m_data[0] = v_a_ref;
            converter.m_data[1] = v_b_ref;
            converter.m_data[2] = v_c_ref;
            converter.m_data[3] = m0;
            converter.m_data[4] = v_dc_diff;
            converter.m_data[5] = v_dc_diff_filtered;

            // write to output registers
            for (uint32_t index = 0; index < num_data; index++)
            {
                // converter.m_r2scpp.data[index].write(cast<double, uint64_t>(converter.m_data[index]));
                converter.m_r2scpp.data[index].write((converter.m_data[index]));
            }

            // send it away
            // kria transfer rate: 100us
            converter.m_r2s->num_data = num_data * 2;
            converter.m_r2s->tkeep    = 0x0000FFFF;

            // trigger connection
            converter.m_r2scpp.ctrl.start.set(true);

            // converter.counter++;
        }

        // static void RTTaskPerf(Converter& converter)
        // {
        //     // for (int index=0; index<50; index++)
        //     // {
        //         asm volatile("isb; dsb sy");
        //         const volatile double meas = 0.5;//static_cast<double>((std::rand() / RAND_MAX - 1) * 100.0);
        //         const volatile double ref = 1.0;//static_cast<double>((std::rand() / RAND_MAX - 1) * 100.0);

        //         converter.m_data[0] = meas;
        //         converter.m_data[1] = ref;

        //         const volatile double act = 0.0;
        //     //     const volatile double act = converter.rst.control(ref, meas);

        //         converter.m_data[2] = act;
        //         asm volatile("isb; dsb sy");
        //     // }
        //     converter.counter++;
        // }

      private:
        int counter{0};

        constexpr static uint32_t    num_data{20};
        std::array<double, num_data> m_data;

        volatile stream_to_reg* m_s2r;
        volatile reg_to_stream* m_r2s;
    };

}   // namespace user
