//! @file
//! @brief Defines the Active Front-End Component implementing simple transformation including two outer and two outer
//! loops, and frame conversion of the output.
//! @author Dominik Arominski

#include "vslib.hpp"

#pragma once

namespace user
{

    class ActiveFrontEnd : vslib::Component
    {

      public:
        ActiveFrontEnd(std::string_view name, vslib::Component& parent)
            : vslib::Component("ActiveFrontEnd", name, parent),
              pll("pll", *this),
              abc_2_dq0("abc_2_dq0", *this),
              dq0_2_abc("dq0_2_abc", *this),
              power_3ph_instant("power_3ph_instant", *this),
              pi_id_ref("pi_id_ref", *this),
              pi_iq_ref("pi_iq_ref", *this),
              pi_vd_ref("pi_vd_ref", *this),
              pi_vq_ref("pi_vq_ref", *this),
              limit("limit", *this),
              inductance(*this, "inductance"),
              frequency(*this, "frequency"),
              v_base(*this, "v_base"),
              i_base(*this, "i_base")
        {
        }

        //! Performs full active front-end transformation from abc voltage and current measurements and reference frame
        //! to calculate the v_ref in abc frame.
        //!
        //! @param v_a a-component of voltage [V]
        //! @param v_b b-component of voltage [V]
        //! @param v_c c-component of voltage [V]
        //! @param i_a a-component of current [A]
        //! @param i_b b-component of current [A]
        //! @param i_c c-component of current [A]
        //! @param p_ref Reference active power
        //! @param q_ref Reference reactive power
        //! @return a, b, and c components of the voltage reference
        std::tuple<double, double, double> control(
            const double v_a, const double v_b, const double v_c, const double i_a, const double i_b, const double i_c,
            const double p_ref, const double q_ref, const double start
        )
        {
            //
            // Measurement and reference frame
            //
            const double wt_pll = pll.synchronise(v_a * m_v_to_pu, v_b * m_v_to_pu, v_c * m_v_to_pu);
            const auto [vd_meas, vq_meas, zero_v]
                = abc_2_dq0.transform(v_a * m_v_to_pu, v_b * m_v_to_pu, v_c * m_v_to_pu, wt_pll);
            const auto [id_meas, iq_meas, zero_i]
                = abc_2_dq0.transform(i_a * m_i_to_pu, i_b * m_i_to_pu, i_c * m_i_to_pu, wt_pll);
            const auto [p_meas, q_meas] = power_3ph_instant.calculate(v_a, v_b, v_c, i_a, i_b, i_c);

            //
            // Outer loops
            //
            const auto id_ref = pi_id_ref.control(start * p_ref, start * p_meas);
            const auto iq_ref = -pi_iq_ref.control(start * q_ref, start * q_meas);

            //
            // Inner loops
            //
            // PI + 2 * ff for each loop
            const auto vd_ref
                = pi_vd_ref.control(start * id_ref, start * id_meas) + vd_meas - i_base * m_wl * m_si_to_pu * iq_meas;
            const auto vq_ref
                = pi_vq_ref.control(start * iq_ref, start * iq_meas) + vq_meas + i_base * m_wl * m_si_to_pu * id_meas;

            //
            // Frame conversion
            //
            const auto vd_ref_lim = limit.limit(-vd_ref);
            const auto vq_ref_lim = limit.limit(-vq_ref);

            return dq0_2_abc.transform(vd_ref_lim, vq_ref_lim, 0.0, wt_pll);
        }

        // Owned Components
        vslib::SRFPLL                       pll;
        vslib::AbcToDq0Transform            abc_2_dq0;
        vslib::Dq0ToAbcTransform            dq0_2_abc;
        vslib::InstantaneousPowerThreePhase power_3ph_instant;
        vslib::PID                          pi_id_ref;
        vslib::PID                          pi_iq_ref;
        vslib::PID                          pi_vd_ref;
        vslib::PID                          pi_vq_ref;
        vslib::LimitRange<double>           limit;

        // Owned Parameters
        vslib::Parameter<double> inductance;   //!< Inductance of the system [H]
        vslib::Parameter<double> frequency;    //!< Current frequency [Hz]
        vslib::Parameter<double> v_base;       //!< Base voltage [V]
        vslib::Parameter<double> i_base;       //!< Base current [A]

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_wl       = 2.0 * std::numbers::pi * frequency.toValidate() * inductance.toValidate();
            // conversion constants, based on base voltage and base current:
            m_v_to_pu  = 1.0 / v_base.toValidate();
            m_si_to_pu = sqrt(3.0 / 2.0) / v_base.toValidate();
            m_pu_to_si = 1.0 / m_si_to_pu;
            m_i_to_pu  = 1.0 / i_base.toValidate();

            return {};
        }

      private:
        double m_wl{0.0};
        double m_v_to_pu{0.0};    //!< voltage to per unit
        double m_si_to_pu{0.0};   //!<
        double m_pu_to_si{0.0};
        double m_i_to_pu{0.0};   //!< current to per unit
    };
}   // namespace user