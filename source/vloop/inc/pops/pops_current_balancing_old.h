//! @file
//! @brief Defines the Component implementing the old approach to current balancing for POPS.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "vslib.h"

namespace user
{
    class CurrentBalancingOld : public vslib::Component
    {
      public:
        CurrentBalancingOld(std::string_view name, vslib::Component& parent)
            : vslib::Component("CurrentBalancingOld", name, parent),
              maverage_a("maverage_a", *this),
              maverage_b("maverage_b", *this),
              maverage_c("maverage_c", *this),
              saturation("saturation_protection", *this),
              avoid_zero_division("avoid_zero_division", *this),
              maverage_notch_frequency(*this, "maverage_notch_frequency"),
              i_base(*this, "i_base"),
              v_max(*this, "v_max")
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
            const double i_a_norm = i_a / i_base;
            const double i_b_norm = i_b / i_base;
            const double i_c_norm = i_c / i_base;
            const double ron_vdc  = m_r_on / avoid_zero_division.limit(2.0 * v_dc / v_max);

            // calculate moving averages for all input currents
            const double i_a_mav = maverage_a.filter(i_a_norm);
            const double i_b_mav = maverage_b.filter(i_b_norm);
            const double i_c_mav = maverage_c.filter(i_c_norm);

            // calculate average of all currents:
            const double i_abc_av = (i_a_mav + i_b_mav + i_c_mav) / 3.0;

            // subtract the current value of each component from the average and set it to it
            const double i_a_balanced = i_abc_av - i_a_mav;
            const double i_b_balanced = i_abc_av - i_b_mav;
            const double i_c_balanced = i_abc_av - i_c_mav;

            // multiply them by ron_vdc to calculate apparent voltage of each component
            const double v_a = i_a_balanced * ron_vdc;
            const double v_b = i_b_balanced * ron_vdc;
            const double v_c = i_c_balanced * ron_vdc;

            // calculate saturation-protected values, subtract them from the original value to calculate a mean
            // voltage of all components and add the mean voltage to each component
            const double v_a_limited = saturation.limit(v_a);
            const double v_b_limited = saturation.limit(v_b);
            const double v_c_limited = saturation.limit(v_c);

            // calculate common mean voltage:
            const double v_mean = ((v_a - v_a_limited) + (v_b - v_b_limited) + (v_c - v_c_limited)) / 3.0;

            // sum limited voltages with the mean voltage
            const double v_a_out = v_a_limited + v_mean;
            const double v_b_out = v_b_limited + v_mean;
            const double v_c_out = v_c_limited + v_mean;

            // finally, calculate indices
            const double m_a = v_a_out + common_modulation;
            const double m_b = v_b_out + common_modulation;
            const double m_c = v_c_out + common_modulation;

            return std::make_tuple(m_a, m_b, m_c);
        }

        // Components owned by this Component
        vslib::BoxFilter<5>       maverage_a;            // moving average filter for a-component, 5th order
        vslib::BoxFilter<5>       maverage_b;            // b-component
        vslib::BoxFilter<5>       maverage_c;            // c-component
        vslib::LimitRange<double> saturation;            // saturation-protection
        vslib::LimitRange<double> avoid_zero_division;   // zero-division protection

        // Parameters owned by this Component
        vslib::Parameter<double> maverage_notch_frequency;   //!< Notch frequency value for moving-average filters
        vslib::Parameter<double> i_base;                     //!< Base current [A], used for normalization
        vslib::Parameter<double> v_max;                      //!< Maximal voltage [V], used for normalization

      private:
        double m_r_on{-0.4};   //! ???
    };
}