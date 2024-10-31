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
              maverage_pos_a("maverage_pos_a", *this),
              maverage_pos_b("maverage_pos_b", *this),
              maverage_pos_c("maverage_pos_c", *this),
              maverage_neg_a("maverage_neg_a", *this),
              maverage_neg_b("maverage_neg_b", *this),
              maverage_neg_c("maverage_neg_c", *this),
              saturation("saturation_protection", *this),
              avoid_zero_division("avoid_zero_division", *this),
              switching_frequency(*this, "switching_frequency"),
              i_base(*this, "i_base"),
              //   control_period(*this, "control_period", 0.0),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
        vslib::BoxFilter<5>                   maverage_pos_a;   // moving average filters
        vslib::BoxFilter<5>                   maverage_pos_b;
        vslib::BoxFilter<5>                   maverage_pos_c;
        vslib::BoxFilter<5>                   maverage_neg_a;
        vslib::BoxFilter<5>                   maverage_neg_b;
        vslib::BoxFilter<5>                   maverage_neg_c;
        vslib::LimitRange<double>             saturation;
        vslib::LimitRange<double>             avoid_zero_division;
        // ...
        // end of your Components

        // Define your Parameters here
        // vslib::Parameter<double> control_period;
        vslib::Parameter<double> switching_frequency;
        vslib::Parameter<double> i_base;
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

        //! Calculates current balancing modulation indices: old calculation.
        //!
        //! @param i_a a-component of the current
        //! @param i_b b-component of the current
        //! @param i_c c-component of the current
        //! @param ron_vdc ???
        //! @param modulation External modulation index
        //! @param positive Whether this is a positive or negative current and index
        //! @return Tuple of a, b, and c modulation indices
        std::tuple<double, double, double> balance_current_old(
            const double i_a, const double i_b, const double i_c, const double ron_vdc, const double modulation,
            const bool positive
        )
        {
            // first, calculate moving average for all input currents
            const double i_a_av = positive ? maverage_pos_a.filter(i_a) : maverage_neg_a.filter(i_a);
            const double i_b_av = positive ? maverage_pos_b.filter(i_b) : maverage_neg_a.filter(i_a);
            const double i_c_av = positive ? maverage_pos_c.filter(i_c) : maverage_neg_a.filter(i_a);

            // calculate average of all currents:
            const double i_abc_av = (i_a_av + i_b_av + i_c_av) / 3.0;

            // subtract the current value of each component from the average and set it to it
            const double i_a_balanced = i_abc_av - i_a_av;
            const double i_b_balanced = i_abc_av - i_b_av;
            const double i_c_balanced = i_abc_av - i_c_av;

            // multiply them by ron_vdc to calculate apparent power of each component
            const double p_a = i_a_balanced * ron_vdc;
            const double p_b = i_a_balanced * ron_vdc;
            const double p_c = i_a_balanced * ron_vdc;

            // calculate saturation-protected values, subtract them from the original value to calculate a mean power of
            // all components and add the mean power to each component
            const double p_a_limited = saturation.limit(p_a);
            const double p_b_limited = saturation.limit(p_b);
            const double p_c_limited = saturation.limit(p_c);

            // calculate average power
            const double p_mean = ((p_a - p_a_limited) + (p_b - p_b_limited) + (p_c - p_c_limited)) / 3.0;

            // sum limited power with the mean power
            const double p_a_out = p_a_limited + p_mean;
            const double p_b_out = p_b_limited + p_mean;
            const double p_c_out = p_c_limited + p_mean;

            // finally, calculate modulation indices
            const double m_a = p_a_out + modulation;
            const double m_b = p_b_out + modulation;
            const double m_c = p_c_out + modulation;

            return std::make_tuple(m_a, m_b, m_c);
        }

        std::tuple<double, double, double> balance_current(
            const double i_a, const double i_b, const double i_c, const double vdc, const double modulation,
            const bool positive
        )
        {
            // first, calculate moving average for all input currents

            const double i_a_av = positive ? maverage_pos_a.filter(i_a) : maverage_neg_a.filter(i_a);
            const double i_b_av = positive ? maverage_pos_b.filter(i_b) : maverage_neg_a.filter(i_a);
            const double i_c_av = positive ? maverage_pos_c.filter(i_c) : maverage_neg_a.filter(i_a);

            // calculate average of all currents:
            const double i_abc_av = (i_a_av + i_b_av + i_c_av) / 3.0;

            // subtract the current value of each component from the average and set it to it
            const double i_a_balanced = i_abc_av - i_a_av;
            const double i_b_balanced = i_abc_av - i_b_av;
            const double i_c_balanced = i_abc_av - i_c_av;

            // multiply mean-subtracted currents by I_base to calculate scaled current of each component
            const double i_a_scaled
                = i_base * switching_frequency
                  * (factors_a[0] * i_a_balanced + factors_a[1] * i_b_balanced + factors_a[2] * i_c_balanced);
            const double i_b_scaled
                = i_a_balanced * i_base * switching_frequency
                  * (factors_b[0] * i_a_balanced + factors_b[1] * i_b_balanced + factors_b[2] * i_c_balanced);
            const double i_c_scaled
                = i_a_balanced * i_base * switching_frequency
                  * (factors_c[0] * i_a_balanced + factors_c[1] * i_b_balanced + factors_c[2] * i_c_balanced);

            // divide power components by vdc_meas scaled to power units
            const double vdc_scaled     = avoid_zero_division.limit(vdc) * m_max_voltage;   // max_voltage = V_2_pu
            const double inv_r_a_scaled = i_a_scaled / vdc_scaled;
            const double inv_r_b_scaled = i_b_scaled / vdc_scaled;
            const double inv_r_c_scaled = i_c_scaled / vdc_scaled;

            // finally, calculate modulation indices
            const double m_a = modulation;
            const double m_b = modulation;
            const double m_c = modulation;
            return std::make_tuple(m_a, m_b, m_c);
        }

        const double          ron{-0.4};
        std::array<double, 3> factors_a{5.4e-3, -1.2e-3, -1.2e-3};
        std::array<double, 3> factors_b{-1.2e-3, 5.4e-3, -1.2e-3};
        std::array<double, 3> factors_c{-1.2e-3, -1.2e-3, 5.4e-3};

        static void RTTask(Converter& converter)
        {
            constexpr uint32_t                num_data      = 40;
            constexpr uint32_t                num_data_half = 20;
            std::array<double, num_data_half> data_in;

            // collect inputs
            for (std::size_t i = 0; i < num_data_half; ++i)
            {
                data_in[i] = cast<uint64_t, double>(converter.m_s2r->data[i].value);
            }

            const double m_idx_pos = data_in[0];   // positive modulation index
            const double m_idx_neg = data_in[1];   // negative modulation index
            const double ip_a      = data_in[2];   // positive current components
            const double ip_b      = data_in[3];
            const double ip_c      = data_in[4];
            const double in_a      = data_in[5];   // negative current components
            const double in_b      = data_in[6];
            const double in_c      = data_in[7];
            const double vdc       = data_in[8];

            // derived values
            const double vdc_meas = vdc * 2.0 / converter.m_max_voltage;
            const double ron_vdc  = converter.ron / converter.avoid_zero_division.limit(vdc_meas);

            // zero outputs to avoid confusion
            for (int index = 0; index < num_data_half; index++)
            {
                data_in[index] = 0.0;   // zeroing as this channel is reused for output
            }

            const auto [m_a_pos, m_b_pos, m_c_pos]
                = converter.balance_current_old(ip_a, ip_b, ip_c, ron_vdc, m_idx_pos, true);
            const auto [m_a_neg, m_b_neg, m_c_neg]
                = converter.balance_current_old(in_a, in_b, in_c, ron_vdc, m_idx_neg, false);

            // set outputs:
            data_in[0] = m_a_pos;
            data_in[1] = m_b_pos;
            data_in[2] = m_c_pos;
            data_in[3] = m_a_neg;
            data_in[4] = m_b_neg;
            data_in[5] = m_c_neg;

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

        // possible Parameters a new Component:
        const double m_max_voltage{5000.0};
    };

}   // namespace user
