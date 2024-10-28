#include "vslib.h"

namespace user
{
    //! Provides all the functionalities to process P80 cyclic data.
    class P80CyclicDataParser
    {
        static constexpr std::array<std::string_view, 30> signal_name
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

        static constexpr std::array<std::string_view, 9> ordinal_numerals
            = {"FIRST", "SECOND", "THIRD", "FOURTH", "FIFTH", "SIXTH", "SEVENTH", "EIGHT", "NINTH"};

      public:
        //! Creates P80 cyclic data-specific parser object.
        P80CyclicDataParser()
        {
            for (int index = 0; index < signal_name.size(); index++)
            {
                m_cyclic_data[signal_name[index]] = -1.0;
            }
        }

        //! Sets a value to the P80 cyclic data array.
        void setCyclicData(size_t index, double value)
        {
            // std::cout << signal_name[index] << " " << value << " ";
            m_cyclic_data[signal_name[index]] = value;
            // std::cout << m_cyclic_data[signal_name[index]] << std::endl;
        }


        double getOpenLoopVref() const
        {
            return m_cyclic_data.at("REF.START.VREF");
        }

        //! Returns cycle duration, depending on the time of the last plateau.
        //!
        //! @return Current cycle's duration, either 2.4 s or 3.6 s.
        double findCycleDuration() noexcept
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
        //! @param plateau_index Index of the plateau to get the reference of
        //! @return Reference value at the requested index
        double getPlateaubyId(const int plateau_index) noexcept
        {
            return (plateau_index == 0) ? m_cyclic_data[fmt::format("REF.FIRST_PLATEAU.REF")]
                                        : m_cyclic_data[fmt::format("REF.PPPL.REF4_{}", plateau_index - 1)];
        }

        //! Sets the plateau id at the current time, or a previous one if the provided time falls between plateaux.
        //!
        //! @param current_time The current value of time within the cycle being played
        int getPlateauId(const double current_time) noexcept
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
            return id;
        }

        double getReference(const double current_time)
        {
            double reference         = 0.0;
            double previous_ref      = 0.0;
            double previous_max_time = 0.0;

            for (int index = 0; index < ordinal_numerals.size(); index++)
            {
                const auto&  numeral       = ordinal_numerals[index];
                const double next_min_time = m_cyclic_data.at(fmt::format("REF.{}_PLATEAU.TIME", numeral));
                const double next_max_time
                    = next_min_time + m_cyclic_data.at(fmt::format("REF.{}_PLATEAU.DURATION", numeral));
                const auto next_ref = getPlateaubyId(index);
                // first, if we fall between plateaux: interpolate the reference
                if (current_time < next_min_time || next_min_time < 0)
                {
                    reference
                        = interpolateToNext(current_time, previous_max_time, previous_ref, next_min_time, next_ref);
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

        //! Finds and sets the time of the last plateau, when the recharge starts.
        double endTimeLastPlateau()
        {
            int index = 0;
            // find first non-set plateau: last plateau is the previous one
            while (index < ordinal_numerals.size())
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

                if (plateau <= 1e-3)   // sometimes the last plateau was supposed to be zero but is ~1e-9
                {
                    break;
                }
                index++;
            }
            const int last_plateau_id = index - 1;
            return m_cyclic_data[fmt::format("REF.{}_PLATEAU.TIME", ordinal_numerals[last_plateau_id])]
                   + m_cyclic_data[fmt::format("REF.{}_PLATEAU.DURATION", ordinal_numerals[last_plateau_id])];
        }


        //! Prints stored cyclic data.
        void printCyclicData()
        {
            for (int index = 0; index < m_cyclic_data.size(); index++)
            {
                std::cout << signal_name[index] << ": " << m_cyclic_data[signal_name[index]] << std::endl;
            }
        }

      private:
        std::map<std::string_view, float> m_cyclic_data;   //! P80 cyclic data from FGC2

        int m_current_plateau_id{0};   //! Current plateau ID

        //! Linearly interpolates the y value based on the provided x value and two nearest points
        //!
        //! @param x Current value of x
        //! @param x1 X-axis value of point1, smaller than x
        //! @param y1 Y-axis value of point1, smaller than x
        //! @param x2 X-axis value of point2, larger than x
        //! @param y2 Y-axis value of point2, larger than x
        //! @return Interpolated Y-axis value
        double interpolateToNext(const double x, const double x1, const double y1, const double x2, const double y2)
        {
            return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
        }
    };

    class POPSDispatcher : public vslib::Component
    {
        enum class OperatingMode
        {
            normal,       //! 6 DCDC, 2 AFE
            degraded_1,   //! 6 DCDC, 1 AFE
            degraded_2    //! 5 DCDC, 2 AFE
        };

      public:
        static constexpr size_t TotalNumberDCDC = 6;

        POPSDispatcher(vslib::IConverter& parent)
            : vslib::Component("POPSDispatcher", "dispatcher", parent),
              magnets_r(*this, "magnets_r"),
              magnets_l(*this, "magnets_l"),
              operating_mode(*this, "operating_mode"),
              parser()
        {
        }

        // ************************************************************
        // Public Parameters

        vslib::Parameter<double>        magnets_r;        //! Magnets' resistance, in Ohms
        vslib::Parameter<double>        magnets_l;        //! Magnets' inductance, in H
        vslib::Parameter<OperatingMode> operating_mode;   //! POPS operating mode:

        // ************************************************************
        // Parser of function to be played

        P80CyclicDataParser parser;   //! Data parser coming from P80 cyclic schema (FGC2)

        // ************************************************************
        // Initialization and verification of Parameters

        void init()
        {
            m_recharge_time = parser.endTimeLastPlateau();
        }

        // ************************************************************
        // Setters

        //! Sets the value of measured Vdc to internal array.
        void setVdcMeas(size_t index, double value)
        {
            m_v_dc_meas[index] = value;
        }

        // Getters

        //! Returns the array with modulation indices.
        const std::array<double, TotalNumberDCDC>& getModulationId() const
        {
            return m_idx;
        }

        //! Returns the number of active number of DCDC converters.
        //!
        //! @param current_time Time in the cycle at which to provide the number of active DCDCs
        //! @return Number of active DCDC converters, 1, 2, or 6.
        int getNdcdc(const double current_time) noexcept
        {
            if (current_time >= m_recharge_time)
            {
                return TotalNumberDCDC;
            }

            const auto currentPlateauId = parser.getPlateauId(current_time);
            int        n_dcdc           = 0;
            if (currentPlateauId == -1)
            {
                // open-loop case, either 1 or 2 DC-DCs are active
                n_dcdc = (parser.getOpenLoopVref() > m_open_loop_limit) ? 2 : 1;
            }
            else
            {
                const double v_estimated = parser.getReference(current_time) * magnets_r;
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
                    n_dcdc = TotalNumberDCDC;
                }
            }
            return n_dcdc;
        }

        //!
        void dispatchVoltage(const double current_time, const double v_ref, const double i_mag_meas) noexcept
        {
            std::fill(m_v_ref_dispatch.begin(), m_v_ref_dispatch.end(), 0.0);

            const double v_r = magnets_r * i_mag_meas;
            const double v_l = v_ref - v_r;

            if (current_time < m_recharge_time)
            {
                const int n_dcdc = getNdcdc(current_time);
                dispatchCycle(v_ref, v_r, v_l, n_dcdc);
            }
            else   // recharge is active
            {
                dispatchRecharging(v_ref, i_mag_meas, v_r, v_l);
            }

            // set the outputs:
            for (int index = 0; index < TotalNumberDCDC; index++)
            {
                m_idx[index] = m_v_ref_dispatch[index] / m_v_dc_meas[index];
            }
        }

      private:
        std::array<double, TotalNumberDCDC> m_v_dc_meas{0.0};      //! Vdc measurements for each DCDC
        std::array<double, TotalNumberDCDC> m_v_ref_dispatch{0};   //! Vref split between DCDCs
        std::array<double, TotalNumberDCDC> m_idx{0.0};            //! Modulation indices for each DCDC
        std::array<double, TotalNumberDCDC> m_dEc{0};              //! Energy needed to recharge DCDC to nominal voltage

        //! Parameters of the converter, will be fixed or provided by Configurator? Or VSlib GUI Parameter?
        const double m_v_min{35};                 //!< Minimum voltage that IGBT can deliver, in V
        const double m_level_1{4 * m_v_min};      //!< Threshold to start using 2 DCDC, below: 1 DCDC
        const double m_level_2{8 * m_v_min};      //!< Threshold to start using 6 DCDC, below: 2 DCDC
        const double m_open_loop_limit{4900.0};   //!< Minimum voltage during the open loop to engage 2 chargers DCDC

        const int m_n_floaters = 4;   //! number of floaters DCDC
        const int m_n_chargers = 2;   //! number of chargers DCDC

        // factors in dispatcher:
        const double m_k{0.5 * 0.247};             // ???
        const double m_Udc_min_floaters{2400.0};   //! min voltage of floater DCDC, in V
        const double m_Udc_max_floaters{5000.0};   //! max voltage of floater DCDC, in V
        const double m_Udc_min_chargers{3100.0};   //! min voltage of charger DCDC, in V
        const double m_Udc_max_chargers{5000.0};   //! max voltage of charger DCDC, in V

        double m_recharge_time{0.0};

        bool m_original_calculation{false};   //! Whether to use the 'original' calculation or not

        void dispatchRecharging(const double v_ref, const double i_mag_meas, const double v_r, const double v_l)
        {
            double kc = 0;
            double kf = 0;

            const double nominal_v2 = std::pow(5000.0, 2);
            for (int index = 0; index < m_dEc.size(); index++)
            {
                const double energy = 0.5 * 0.247 * (nominal_v2 - std::pow(m_v_dc_meas[index], 2));
                m_dEc[index]        = (energy > 0) ? energy : 0.0;
            }
            // total energy required to charge chargers to nominal voltage:
            double Ec;
            if (operating_mode == OperatingMode::normal)
            {
                Ec = m_dEc[0] + m_dEc[1];
            }
            else if (operating_mode == OperatingMode::degraded_1)
            {
                Ec = m_dEc[0];
            }
            // total energy required to charge floaters to nominal voltage:
            double Ef = m_dEc[2] + m_dEc[3] + m_dEc[4] + m_dEc[5];
            if (operating_mode == OperatingMode::degraded_1)
            {
                Ef += m_dEc[1];
            }

            if (Ef > 0 && i_mag_meas > 0)   // floaters not completely charged
            {
                kf = 2.0 * Ef / (magnets_l * std::pow(i_mag_meas, 2));
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
                int n_chargers = m_n_chargers;
                if (operating_mode == OperatingMode::normal)
                {
                    // chargers:
                    if (Ec > 0)
                    {
                        m_v_ref_dispatch[0] = 0.5 * v_r + kc * v_l * (m_dEc[0] / Ec);
                        m_v_ref_dispatch[1] = 0.5 * v_r + kc * v_l * (m_dEc[1] / Ec);
                    }
                    else
                    {
                        m_v_ref_dispatch[0] = 0.5 * (v_r + v_l * kc);
                        m_v_ref_dispatch[1] = 0.5 * (v_r + v_l * kc);
                    }
                }
                else if (operating_mode == OperatingMode::degraded_1)
                {
                    n_chargers = 1;
                    // chargers:
                    if (Ec > 0)
                    {
                        m_v_ref_dispatch[0] = v_r + kc * v_l * (m_dEc[0] / Ec);
                    }
                    else
                    {
                        m_v_ref_dispatch[0] = (v_r + v_l * kc);
                    }
                }
                // floaters:
                for (int index = n_chargers; index < m_dEc.size(); index++)
                {
                    m_v_ref_dispatch[index] = v_l * kf * (m_dEc[index] / Ef);
                }
            }
            else
            {
                // chargers:
                if (operating_mode == OperatingMode::normal)
                {
                    m_v_ref_dispatch[0] = 0.5 * v_ref;
                    m_v_ref_dispatch[1] = 0.5 * v_ref;
                }
                else if (operating_mode == OperatingMode::degraded_1)
                {
                    m_v_ref_dispatch[0] = v_ref;
                    m_v_ref_dispatch[1] = 0.0;
                }
                // floaters:
                m_v_ref_dispatch[2] = 0.0;
                m_v_ref_dispatch[3] = 0.0;
                m_v_ref_dispatch[4] = 0.0;
                m_v_ref_dispatch[5] = 0.0;
            }
        }

        void dispatchCycle(const double v_ref, const double v_r, const double v_l, const int n_dcdc)
        {
            double kf = 0;
            double kc = 0;
            if (n_dcdc == 1)
            {
                m_v_ref_dispatch[0] = v_ref;
            }
            else if (n_dcdc == 2)
            {
                if (operating_mode == OperatingMode::normal)
                {
                    m_v_ref_dispatch[0] = 0.5 * v_ref;
                    m_v_ref_dispatch[1] = 0.5 * v_ref;
                }
                else if (operating_mode == OperatingMode::degraded_1)
                {
                    m_v_ref_dispatch[0] = v_ref;
                }
            }
            else   // n_dcdc == 6
            {
                if (operating_mode == OperatingMode::normal)
                {
                    if (m_original_calculation)
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

                        m_v_ref_dispatch[0] = v_ref * (1 - kf) / m_n_chargers;
                        m_v_ref_dispatch[1] = m_v_ref_dispatch[0];
                        m_v_ref_dispatch[5] = m_v_ref_dispatch[4] = m_v_ref_dispatch[3] = m_v_ref_dispatch[2]
                            = v_ref * kf / m_n_floaters;
                    }
                    else
                    {
                        // fixed-factors approach
                        if (fabs(v_l) < m_v_min)
                        {
                            m_v_ref_dispatch[0] = v_ref * 0.35;
                            m_v_ref_dispatch[1] = m_v_ref_dispatch[0];
                            m_v_ref_dispatch[5] = m_v_ref_dispatch[4] = m_v_ref_dispatch[3] = m_v_ref_dispatch[2]
                                = v_ref * 0.075;
                        }
                        else
                        {
                            m_v_ref_dispatch[0] = 0.5 * v_r + v_l / 6.0;
                            m_v_ref_dispatch[1] = m_v_ref_dispatch[0];
                            m_v_ref_dispatch[5] = m_v_ref_dispatch[4] = m_v_ref_dispatch[3] = m_v_ref_dispatch[2]
                                = v_l / 6.0;
                        }
                    }
                }
                else if (operating_mode == OperatingMode::degraded_1)
                {
                    m_v_ref_dispatch[0] = 0.7 * v_r + v_l / 6.0;
                    m_v_ref_dispatch[5] = m_v_ref_dispatch[4] = m_v_ref_dispatch[3] = m_v_ref_dispatch[2]
                        = m_v_ref_dispatch[1]                                       = 0.06 * v_r + v_l / 6.0;
                }
            }
        }
    };
}