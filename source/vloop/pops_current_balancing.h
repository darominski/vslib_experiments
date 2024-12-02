//! @file
//! @brief Defines the Component implementing the new approach to current balancing for POPS.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "vslib.h"

namespace user
{
    class CurrentBalancing : public vslib::Component
    {
      public:
        //! Constructs a CurrentBalancing Component.
        //!
        //! @param name Name of this Component
        //! @param parent Reference to the parent of this Component
        CurrentBalancing(std::string_view name, vslib::Component& parent)
            : vslib::Component("CurrentBalancing", name, parent),
              maverage_a_5("maverage_a_5", *this),
              maverage_b_5("maverage_b_5", *this),
              maverage_c_5("maverage_c_5", *this),
              maverage_a_3("maverage_a_3", *this),
              maverage_b_3("maverage_b_3", *this),
              maverage_c_3("maverage_c_3", *this),
              saturation("saturation_protection", *this),
              avoid_zero_division("avoid_zero_division", *this),
              maverage_notch_frequency(*this, "maverage_notch_frequency"),
              fifth_filter_order(*this, "fifth_filter_order"),
              i_base(*this, "i_base"),
              v_max(*this, "v_max"),
              a_factors(*this, "a_factors"),
              b_factors(*this, "b_factors"),
              c_factors(*this, "c_factors")
        {
        }

        //! @param i_a a-component of the current [A]
        //! @param i_b b-component of the current [A]
        //! @param i_c c-component of the current [A]
        //! @param v_dc DC Voltage [V]
        //! @param modulation Common modulation index
        //! @return Tuple of balanced a, b, and c modulation indices
        std::tuple<double, double, double>
        balance(const double i_a, const double i_b, const double i_c, const double v_dc, const double common_modulation)
        {
            // first, normalize inputs
            const double i_a_norm   = i_a / i_base;
            const double i_b_norm   = i_b / i_base;
            const double i_c_norm   = i_c / i_base;
            const double vdc_scaled = avoid_zero_division.limit(2.0 * v_dc / v_max) * v_max;

            // calculate moving averages for all input currents
            const double i_a_mav = fifth_filter_order ? maverage_a_5.filter(i_a_norm) : maverage_a_3.filter(i_a_norm);
            const double i_b_mav = fifth_filter_order ? maverage_b_5.filter(i_b_norm) : maverage_b_3.filter(i_b_norm);
            const double i_c_mav = fifth_filter_order ? maverage_c_5.filter(i_c_norm) : maverage_c_3.filter(i_c_norm);

            // calculate average of all currents
            const double i_abc_av = (i_a_mav + i_b_mav + i_c_mav) / 3.0;

            // subtract the the average from each component
            const double i_a_balanced = i_a_mav - i_abc_av;
            const double i_b_balanced = i_b_mav - i_abc_av;
            const double i_c_balanced = i_c_mav - i_abc_av;

            // multiply mean-subtracted currents by I_base and matrix elements to calculate scaled current of each
            // component
            const double i_a_scaled
                = i_base * maverage_notch_frequency
                  * (factors_a[0] * i_a_balanced + factors_a[1] * i_b_balanced + factors_a[2] * i_c_balanced);
            const double i_b_scaled
                = i_base * maverage_notch_frequency
                  * (factors_b[0] * i_a_balanced + factors_b[1] * i_b_balanced + factors_b[2] * i_c_balanced);
            const double i_c_scaled
                = i_base * maverage_notch_frequency
                  * (factors_c[0] * i_a_balanced + factors_c[1] * i_b_balanced + factors_c[2] * i_c_balanced);

            // divide volate components by vdc_meas scaled to voltage units
            const double inv_a_scaled = i_a_scaled / vdc_scaled;
            const double inv_b_scaled = i_b_scaled / vdc_scaled;
            const double inv_c_scaled = i_c_scaled / vdc_scaled;

            // finally, calculate modulation indices including the common modulation index
            const double m_a = inv_a_scaled + common_modulation;
            const double m_b = inv_b_scaled + common_modulation;
            const double m_c = inv_c_scaled + common_modulation;

            return std::make_tuple(m_a, m_b, m_c);
        }

        // Components owned by this Component
        vslib::BoxFilter<5>       maverage_a_5;          //!< moving average filter for a-component, 5th order
        vslib::BoxFilter<5>       maverage_b_5;          //!< b-component
        vslib::BoxFilter<5>       maverage_c_5;          //!< c-component
        vslib::BoxFilter<2>       maverage_a_3;          //!< moving average filter for a-component, 2nd order
        vslib::BoxFilter<2>       maverage_b_3;          //!< b-component
        vslib::BoxFilter<2>       maverage_c_3;          //!< c-component
        vslib::LimitRange<double> saturation;            //!< saturation-protection
        vslib::LimitRange<double> avoid_zero_division;   //!< zero-division protection

        // Parameters owned by this Component
        vslib::Parameter<double> maverage_notch_frequency;   //!< Notch frequency value for moving-average filters
        //! Whether to use fifth (true) or second (false) order moving average filter
        vslib::Parameter<bool>   fifth_filter_order;
        vslib::Parameter<double> i_base;   //!< Base current [A], used for normalization
        vslib::Parameter<double> v_max;    //!< Maximal voltage [V], used for normalization
        //! First column of the scaling matrix for balancing current
        vslib::Parameter<std::array<double, 3>> a_factors;
        //! Second column of the scaling matrix for balancing current
        vslib::Parameter<std::array<double, 3>> b_factors;
        //! Third column of the scaling matrix for balancing current
        vslib::Parameter<std::array<double, 3>> c_factors;

        //! Sets the matrix factos to a local copy for faster access
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            factors_a[0] = a_factors.toValidate()[0];
            factors_a[1] = a_factors.toValidate()[1];
            factors_a[2] = a_factors.toValidate()[2];

            factors_b[0] = b_factors.toValidate()[0];
            factors_b[1] = b_factors.toValidate()[1];
            factors_b[2] = b_factors.toValidate()[2];

            factors_c[0] = c_factors.toValidate()[0];
            factors_c[1] = c_factors.toValidate()[1];
            factors_c[2] = c_factors.toValidate()[2];

            return {};
        }

      private:
        //! Local copy of the first column of the scaling matrix for balancing current
        std::array<double, 3> factors_a;
        //! Local copy of the second column of the scaling matrix for balancing current
        std::array<double, 3> factors_b;
        //! Local copy of the third column of the scaling matrix for balancing current
        std::array<double, 3> factors_c;
    };
}