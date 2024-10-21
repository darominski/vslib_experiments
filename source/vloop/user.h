#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
#include "vslib.h"

namespace user
{
    static std::array<std::string, 30> signal_name
        = {"REF_USER",
           "REF_USER",
           "REF.START.VREF",
           "REF.FIRST_PLATEAU.REF",
           "REF.FIRST_PLATEAU.TIME",
           "REF.FIRST_PLATEAU.DURATION",
           "REF.PPPL.REF4_0",
           "REF.SECOND_PLATEAU.TIME",
           "REF.SECOND_PLATEAU.DURATION",
           "REF.PPPL.REF4_1",
           "REF.THIRD_PLATEAU.TIME",
           "REF.THIRD_PLATEAU.DURATION",
           "REF.PPPL.REF4_2",
           "REF.FOURTH_PLATEAU.TIME",
           "REF.FOURTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_3",
           "REF.FIFTH_PLATEAU.TIME",
           "REF.FIFTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_4",
           "REF.SIXTH_PLATEAU.TIME",
           "REF.SIXTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_5",
           "REF.SEVENTH_PLATEAU.TIME",
           "REF.SEVENTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_6",
           "REF.EIGHTH_PLATEAU.TIME",
           "REF.EIGHTH_PLATEAU.DURATION",
           "REF.PPPL.REF4_7",
           "REF.NINTH_PLATEAU.TIME",
           "REF.NINTH_PLATEAU.DURATION"};

    static std::array<std::string, 9> ordinal_numerals
        = {"FIRST", "SECOND", "THIRD", "FOURTH", "FIFTH", "SIXTH", "SEVENTH", "EIGHT", "NINTH"};

    class Converter : public vslib::IConverter
    {
      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              m_interrupt_id{121 + 0},   // Jonas's definition
              interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              control_period(*this, "control_period", 0.0),
              m_s2r(reinterpret_cast<volatile stream_to_reg*>(0xA0200000)),
              m_r2s(reinterpret_cast<volatile reg_to_stream*>(0xA0100000))
        {
            // initialize all your objects that need initializing
            for (int index = 0; index < 30; index++)
            {
                m_cyclic_data[signal_name[index]] = -1.0;
            }
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;

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

        void print_cyclic_data()
        {
            for (int index = 0; index < 30; index++)
            {
                std::cout << signal_name[index] << ": " << m_cyclic_data[signal_name[index]] << std::endl;
            }
        }

        double find_cycle_duration()
        {
            for (int index = 8; index > 0; index--)
            {
                const auto&  numeral     = ordinal_numerals[index];
                const double min_plateau = m_cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", numeral)];
                const double max_plateau = min_plateau + m_cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", numeral)];
                if (min_plateau > 0 && max_plateau > 0)
                {
                    return (max_plateau > 2.3) ? 3.6 : 2.4;
                }
            }
        }

        //! Returns the plateau reference value when provided with the index of that reference plateau value.
        //!
        //! @param
        double get_plateau_by_index(const int plateau_index)
        {
            return (plateau_index == 0) ? m_cyclic_data[fmt::format("REF.FIRST_PLATEAU.REF")]
                                        : m_cyclic_data[fmt::format("REF.PPPL.REF4_{}", plateau_index - 1)];
        }

        void get_plateau_id(const double current_time)
        {
            int id = m_current_plateau_id;
            for (int index = m_current_plateau_id; index < 9; index++)
            {
                if (current_time < m_cyclic_data["REF.FIRST_PLATEAU.TIME"]
                    || (m_cyclic_data["REF.FIRST_PLATEAU.TIME"] == -1 || m_cyclic_data["REF.FIRST_PLATEAU.TIME"] == 0))
                {
                    id = -1;
                    break;
                }
                else if (index < 0)
                {
                    continue;
                }
                const auto&  numeral    = ordinal_numerals[index];
                const double start_time = m_cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", numeral)];
                const double duration   = m_cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", numeral)];
                const double end_time   = start_time + duration;

                if (current_time < start_time)
                {
                    // ramp-down or ramp-up to the next plateau
                    id = index - 1;
                    break;
                }
                else if (current_time >= start_time && current_time < end_time)
                {
                    // plateau region
                    id = index;
                    break;
                }
            }
            m_current_plateau_id = id;
        }

        //! Finds and sets the time of the last plateau, when the recharge starts.
        void end_time_of_last_plateau()
        {
            int index = 0;
            // find first non-set plateau: last plateau is the previous one
            while (index < 9)
            {
                double plateau = 0;
                if (index == 0)
                {
                    plateau = m_cyclic_data[fmt::format("REF.FIRST_PLATEAU.REF")];
                }
                else
                {
                    plateau = m_cyclic_data[fmt::format("REF.PPPL.REF4_{}", index - 1)];
                }

                if (plateau <= 1e-3)
                {
                    break;
                }
                index++;
            }
            const int last_plateau_id = index - 1;
            m_recharge_time
                = 1e-3 + m_cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", ordinal_numerals[last_plateau_id])]
                  + m_cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", ordinal_numerals[last_plateau_id])];
        }

        //! Linearly interpolates the y value based on the provided x value and two nearest points
        double interpolate_to_next(const double x, const double x1, const double y1, const double x2, const double y2)
        {
            return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
        }

        double get_reference(const double current_time)
        {
            double reference         = 0.0;
            double previous_ref      = 0.0;
            double previous_max_time = 0.0;

            for (int index = 0; index < 9; index++)
            {
                const auto&  numeral       = ordinal_numerals[index];
                const double next_min_time = m_cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", numeral)];
                const double next_max_time
                    = next_min_time + m_cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", numeral)];
                const auto next_ref = get_plateau_by_index(index);
                // first, if we fall between plateaux: interpolate the reference
                if (current_time < next_min_time)
                {
                    reference
                        = interpolate_to_next(current_time, previous_max_time, previous_ref, next_min_time, next_ref);
                    break;
                }
                // if we are in a plateau: reference is the given reference
                else if (current_time >= next_min_time && current_time < next_max_time)
                {
                    reference = next_ref;
                    break;
                }
                // else: reference is 0
                previous_ref      = next_ref;
                previous_max_time = next_max_time;
            }
            return reference;
        }

        //! Returns the number of active number of DCDC converters.
        //!
        //! @param current_time Time in the cycle at which to provide the number of active DCDCs
        //! @return Number of active DCDC converters, 1, 2, or 6.
        int get_n_dcdc(const double current_time)
        {
            int n_dcdc = 0;
            if (current_time >= m_recharge_time)
            {
                n_dcdc = 6;
                return n_dcdc;
            }

            get_plateau_id(current_time);
            if (m_current_plateau_id == -1)
            {
                // open-loop case, either 1 or 2 DC-DCs are active
                n_dcdc = (m_cyclic_data["REF.START.VREF"] > 4900.0) ? 2 : 1;
            }
            else
            {
                const double v_estimated = get_reference(current_time) * m_r_mag;
                if (v_estimated <= m_level_1)
                {
                    n_dcdc = 1;
                }
                else if (v_estimated > m_level_1 && v_estimated <= m_level_2)
                {
                    n_dcdc = 2;
                }
                else
                {
                    n_dcdc = 6;
                }
            }
            return n_dcdc;
        }

        void pops_dispatcher(
            const double current_time, const double v_ref, const double i_mag_meas, std::array<double, 8>& idx
        )
        {
            const bool original_calculation = false;

            // initialize the indices and kc, kf to zero:
            double v_ref_1 = 0;
            double v_ref_2 = 0;
            double v_ref_3 = 0;
            double v_ref_4 = 0;
            double v_ref_5 = 0;
            double v_ref_6 = 0;
            double kc      = 0;
            double kf      = 0;

            const int    n_dcdc = get_n_dcdc(current_time);
            const double v_r    = m_r_mag * i_mag_meas;
            const double v_l    = v_ref - v_r;

            if (current_time < m_recharge_time)
            {
                if (n_dcdc == 1)
                {
                    v_ref_1 = v_ref;
                    // the rest remains unchanged
                }
                else if (n_dcdc == 2)
                {
                    v_ref_1 = 0.5 * v_ref;
                    v_ref_2 = 0.5 * v_ref;
                    // the rest remain unchanged
                }
                else   // n_dcdc == 6
                {
                    if (original_calculation)
                    {
                        // assuming original calculation:
                        // energy needed to bring floaters to nominal voltage:
                        const double Ef
                            = m_n_floaters * m_k * (std::pow(m_Udc_max_chargers, 2) - std::pow(m_Udc_min_chargers, 2));
                        // energy needed to bring chargers to nominal voltage:
                        const double Ech
                            = m_n_chargers * m_k * (std::pow(m_Udc_max_floaters, 2) - std::pow(m_Udc_min_floaters, 2));

                        const double E = Ef + Ech;
                        kf             = Ef / E;
                        kc             = Ech / E;

                        v_ref_1 = v_ref * (1 - kf) / m_n_chargers;
                        v_ref_2 = v_ref_1;
                        v_ref_3 = v_ref * kf / m_n_floaters;
                        v_ref_4 = v_ref_3;
                        v_ref_5 = v_ref_3;
                        v_ref_6 = v_ref_3;
                    }
                    else
                    {
                        // fixed-factors approach
                        if (fabs(v_l) < m_v_min)
                        {
                            v_ref_1 = v_ref * 0.3;
                            v_ref_2 = v_ref_1;
                            v_ref_3 = v_ref * 0.1;
                            v_ref_4 = v_ref_3;
                            v_ref_5 = v_ref_3;
                            v_ref_6 = v_ref_3;
                        }
                        else
                        {
                            v_ref_1 = 0.5 * v_r + 0.1 * v_l;
                            v_ref_2 = v_ref_1;
                            v_ref_3 = 0.2 * v_l;
                            v_ref_4 = v_ref_3;
                            v_ref_5 = v_ref_3;
                            v_ref_6 = v_ref_3;
                        }
                    }
                }
            }
            else   // recharge is active
            {
                const double          nominal_v2 = std::pow(5000.0, 2);
                std::array<double, 6> dEc{0};   // n dc dc is const
                for (int index = 0; index < dEc.size(); index++)
                {
                    const double energy = 0.5 * 0.247 * (nominal_v2 - std::pow(m_v_dc_meas[index], 2));
                    dEc[index]          = (energy > 0) ? energy : 0.0;
                }
                // total energy required to charge chargers to nominal voltage:
                double Ec = (dEc[0] + dEc[1]);
                if (Ec < 0)   // could be negative if AFE is too fast
                {
                    Ec = 0;
                }
                // total energy required to charge floaters to nominal voltage:
                const double Ef = dEc[2] + dEc[3] + dEc[4] + dEc[5];

                if (Ef > 0 && i_mag_meas > 0)   // floaters not completely charged
                {
                    kf = 2.0 * Ef / (m_l_mag * std::pow(i_mag_meas, 2));
                    if (kf > 1.0)
                    {
                        kf = 1.0;
                    }
                    kc = 1.0 - kf;
                }
                else
                {
                    kf = 0;
                    kc = 1;
                }

                if (kf > 0)
                {
                    // chargers:
                    if (Ec > 0)
                    {
                        v_ref_1 = 0.5 * v_r + kc * v_l * (dEc[0] / Ec);
                        v_ref_2 = 0.5 * v_r + kc * v_l * (dEc[1] / Ec);
                    }
                    else
                    {
                        v_ref_1 = 0.5 * (v_r + v_l * kc);
                        v_ref_2 = 0.5 * (v_r + v_l * kc);
                    }
                    // floaters:
                    v_ref_3 = v_l * kf * (dEc[2] / Ef);
                    v_ref_4 = v_l * kf * (dEc[3] / Ef);
                    v_ref_5 = v_l * kf * (dEc[4] / Ef);
                    v_ref_6 = v_l * kf * (dEc[5] / Ef);
                }
                else
                {
                    // chargers:
                    v_ref_1 = 0.5 * v_ref;
                    v_ref_2 = 0.5 * v_ref;
                    // floaters:
                    v_ref_3 = 0.0;
                    v_ref_4 = 0.0;
                    v_ref_5 = 0.0;
                    v_ref_6 = 0.0;
                }
            }

            // set the outputs:
            idx[0] = v_ref_1 / m_v_dc_meas[0];
            idx[1] = v_ref_2 / m_v_dc_meas[1];
            idx[2] = v_ref_3 / m_v_dc_meas[2];
            idx[3] = v_ref_4 / m_v_dc_meas[3];
            idx[4] = v_ref_5 / m_v_dc_meas[4];
            idx[5] = v_ref_6 / m_v_dc_meas[5];
            idx[6]
                = kc;   // only calculated and relevant when rechargind or n_dcdc is 6 and original calculation is used
            idx[7]
                = kf;   // only calculated and relevant when rechargind or n_dcdc is 6 and original calculation is used
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
            for (int index = 0; index < converter.m_v_dc_meas.size(); index++)
            {
                converter.m_v_dc_meas[index] = data_in[2 + index];
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
                converter.m_cyclic_data[signal_name[converter.c_tim]] = cyclic_data_input;
            }

            const double current_time     = converter.c_tim * converter.control_period;   // inside a cycle
            double       i_meas_estimated = 0.0;
            if (converter.c_tim > 4)   // c_tim > 4, we can start outputting reference
            {
                i_meas_estimated = converter.get_reference(current_time);
            }
            data_in[2] = i_meas_estimated;

            if (converter.c_tim == 30)
            {
                // converter.print_cyclic_data();
                converter.end_time_of_last_plateau();
            }

            if (converter.c_tim >= 30)
            {
                std::array<double, 8> dispatcher_data{0.0};
                converter.pops_dispatcher(current_time, v_ref, i_meas_estimated, dispatcher_data);
                for (int index = 0; index < 8; index++)
                {
                    data_in[index + 3] = dispatcher_data[index];
                }
            }

            data_in[11] = converter.get_n_dcdc(current_time);

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

        unsigned int                 c_tim{0};
        std::map<std::string, float> m_cyclic_data;
        std::array<double, 6>        m_v_dc_meas{0.0};

        //! Parameters of the converter, will be fixed or provided by Configurator? Or VSlib GUI Parameter?
        const double m_r_mag{0.32};            //!< Magnets' resistance, in Ohm
        const double m_l_mag{0.97};            //!< Magnets' inductance, in H
        const double m_v_min{35};              //!< Minimum voltage that IGBT can deliver, in V
        const double m_level_1{4 * m_v_min};   //!< Threshold to start using 2 DCDC, below: 1 DCDC
        const double m_level_2{8 * m_v_min};   //!< Threshold to start using 6 DCDC, below: 2 DCDC
        const int    m_n_floaters = 4;         // number of floaters
        const int    m_n_chargers = 2;         // number of chargers

        // factors in dispatcher:
        const double m_k{0.5 * 0.247};   // ???
        const double m_Udc_min_floaters{2400.0};
        const double m_Udc_max_floaters{5000.0};
        const double m_Udc_min_chargers{3100.0};
        const double m_Udc_max_chargers{5000.0};

        int    m_current_plateau_id{0};
        double m_recharge_time{0.0};
    };

}   // namespace user
