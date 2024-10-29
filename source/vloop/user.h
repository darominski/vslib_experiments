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
              pll("pll", *this),
              pi_id_ref("pi_id_ref", *this),
              pi_iq_ref("pi_iq_ref", *this),
              pi_vd_ref("pi_vd_ref", *this),
              pi_vq_ref("pi_vq_ref", *this),
              limit("limit", *this),
              abc_2_dq0("abc_2_dq0", *this),
              dq0_2_abc("dq0_2_abc", *this),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
        vslib::SRFPLL                         pll;
        vslib::PID                            pi_id_ref;
        vslib::PID                            pi_iq_ref;
        vslib::PID                            pi_vd_ref;
        vslib::PID                            pi_vq_ref;
        vslib::LimitRange<double>             limit;
        vslib::AbcToDq0Transform              abc_2_dq0;
        vslib::Dq0ToAbcTransform              dq0_2_abc;
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
        constexpr static int n_elements     = 10'000;

        void backgroundTask() override
        {
            // reset the PID every 2 minutes
            // if(!m_recently_used)
            // {
            //     sleep(120);
            //     std::cout << "resetting pid\n";
            //     pid.reset();
            // }
            // m_recently_used = false;
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

        static constexpr double inv_sqrt_3 = 1.0 / sqrt(3);

        static constexpr double V_base = 1950.0;
        static constexpr double i_base = 3300.0;

        static constexpr double si_2_pu = sqrt(3.0 / 2.0) / V_base;
        static constexpr double pu_2_si = 1.0 / si_2_pu;
        static constexpr double v_2_pu  = si_2_pu;
        static constexpr double i_2_pu  = 1.0 / i_base;

        static constexpr double inductance = 0.7e-3;
        static constexpr double wL         = 2 * std::numbers::pi * 50.0 * inductance;

        static constexpr double p_gain = sqrt(2.0 / 3.0) / (V_base * i_base);
        static constexpr double q_gain = sqrt(2.0 / 3.0) / (V_base * i_base);

        std::tuple<double, double> power_3ph_instantaneous(
            const double v_a, const double v_b, const double v_c, const double i_a, const double i_b, const double i_c,
            const double p_gain, const double q_gain
        )
        {
            const double p_meas = (v_a * i_a + v_b * i_b + v_c * i_c);
            const double v_ab   = v_a - v_b;
            const double v_bc   = v_b - v_c;
            const double v_ca   = v_c - v_a;

            const double q_meas = (i_a * v_bc + i_b * v_ca + i_c * v_ab) * inv_sqrt_3;
            return {p_meas * p_gain, q_meas * q_gain};
        }

        static void RTTask(Converter& converter)
        {
            // TEST 6: Active Front-End
            constexpr uint32_t                num_data      = 40;
            constexpr uint32_t                num_data_half = 20;
            std::array<double, num_data_half> data_in;

            for (std::size_t i = 0; i < num_data_half; ++i)
            {
                data_in[i] = cast<uint64_t, double>(converter.m_s2r->data[i].value);
            }

            const double start = data_in[0];
            const double p_ref = data_in[1];
            const double q_ref = data_in[2];
            const double v_a   = data_in[3];
            const double v_b   = data_in[4];
            const double v_c   = data_in[5];
            const double i_a   = data_in[6];
            const double i_b   = data_in[7];
            const double i_c   = data_in[8];

            //
            // Measurement and reference frame
            //
            const double wt_pll = converter.pll.balance(v_a * si_2_pu, v_b * si_2_pu, v_c * si_2_pu);
            const auto [vd_meas, vq_meas, zero_v]
                = converter.abc_2_dq0.transform(v_a * v_2_pu, v_b * v_2_pu, v_c * v_2_pu, wt_pll);
            const auto [id_meas, iq_meas, zero_i]
                = converter.abc_2_dq0.transform(i_a * i_2_pu, i_b * i_2_pu, i_c * i_2_pu, wt_pll);
            const auto [p_meas, q_meas]
                = converter.power_3ph_instantaneous(v_a, v_b, v_c, i_a, i_b, i_c, p_gain, q_gain);

            data_in[0] = wt_pll;
            data_in[1] = vd_meas;
            data_in[2] = vq_meas;
            data_in[3] = id_meas;
            data_in[4] = iq_meas;
            data_in[5] = p_meas;
            data_in[6] = q_meas;

            //
            // Outer loops
            //
            const auto id_ref = converter.pi_id_ref.control(start * p_ref, start * p_meas);
            const auto iq_ref = -converter.pi_iq_ref.control(start * q_ref, start * q_meas);

            data_in[7] = id_ref;
            data_in[8] = iq_ref;

            //
            // Inner loops
            //
            // PI + 2 * ff for each loop
            const auto vd_ref = converter.pi_vd_ref.control(start * id_ref, start * id_meas) + vd_meas
                                - i_base * wL * si_2_pu * iq_meas;
            const auto vq_ref = converter.pi_vq_ref.control(start * iq_ref, start * iq_meas) + vq_meas
                                + i_base * wL * si_2_pu * id_meas;
            data_in[9]  = vd_ref;
            data_in[10] = vq_ref;

            //
            // Frame convertion
            //
            const auto vd_ref_lim               = converter.limit.limit(-vd_ref);
            const auto vq_ref_lim               = converter.limit.limit(-vq_ref);
            const auto [vref_a, vref_b, vref_c] = converter.dq0_2_abc.transform(vd_ref_lim, vq_ref_lim, 0.0, wt_pll);

            data_in[11] = vref_a;
            data_in[12] = vref_b;
            data_in[13] = vref_c;

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
            // converter.m_recently_used = true;
            converter.counter++;
        }

        // bool m_recently_used{false};

      private:
        int m_interrupt_id;

        volatile stream_to_reg* m_s2r;
        volatile reg_to_stream* m_r2s;
    };

}   // namespace user
