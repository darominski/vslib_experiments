//! @file
//! @brief Defines the Active Front-End Component with active control in form of RST and voltage balancing.
//! @author Dominik Arominski

#include "vslib.hpp"

#pragma once

namespace user
{

    class ActiveFrontEndVdcBalance : vslib::Component
    {

      public:
        //! Constructs ActiveFrontEnd with RST active control object and Vdc balancing.
        //!
        //! @param name Name of this Component
        //! @param parent Parent of this Component
        ActiveFrontEndVdcBalance(std::string_view name, vslib::Component& parent)
            : vslib::Component("ActiveFrontEndVdcBalance", name, parent),
              pll("pll", *this),
              abc_to_dq0_v("abc_to_dq0_voltage", *this, 100'000),
              abc_to_dq0_i("abc_to_dq0_current", *this, 100'000),
              dq0_to_abc("dq0_to_abc", *this, 100'000),
              power_3ph_instant("power_3ph_instant", *this),
              rst_outer_vdc("rst_outer_vdc", *this),
              rst_outer_id("rst_outer_id", *this),
              rst_outer_iq("rst_outer_iq", *this),
              rst_inner_vd("rst_inner_vd", *this),
              rst_inner_vq("rst_inner_vq", *this),
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
        //! @param regulation_on 1 or 0, depending whether the regulation is to be used or not
        //! @return a, b, and c components of the voltage reference
        std::tuple<double, double, double> vdc_control(
            const double v_a, const double v_b, const double v_c, const double i_a, const double i_b, const double i_c,
            const double v_dc_ref, const double v_dc_meas, const double q_ref, const double regulation_on = 1.0
        )
        {
            //
            // Synchronisation, measurement, and change of reference frame
            //
            const auto wt_pll = pll.synchronise(
                regulation_on * v_a * m_si_to_pu, regulation_on * v_b * m_si_to_pu, regulation_on * v_c * m_si_to_pu
            );
            const auto [vd_meas, vq_meas, zero_v] = abc_to_dq0_v.transform(
                regulation_on * v_a * m_si_to_pu, regulation_on * v_b * m_si_to_pu, regulation_on * v_c * m_si_to_pu,
                wt_pll
            );
            const auto [id_meas, iq_meas, zero_i] = abc_to_dq0_i.transform(
                regulation_on * i_a * m_i_to_pu, regulation_on * i_b * m_i_to_pu, regulation_on * i_c * m_i_to_pu,
                wt_pll
            );
            const auto [p_meas, q_meas] = power_3ph_instant.calculate(
                regulation_on * v_a, regulation_on * v_b, regulation_on * v_c, regulation_on * i_a, regulation_on * i_b,
                regulation_on * i_c
            );

            double p_ref = 0;
            if (regulation_on > 0)
            {
                // needs to not run until regulation is set to ON
                //
                // Outer loop: Vdc control
                //
                p_ref = rst_outer_vdc.control(regulation_on * pow(v_dc_ref, 2), regulation_on * pow(v_dc_meas, 2));
            }

            //
            // Outer loop: power regulation
            // 2 RSTs
            const auto id_ref = rst_outer_id.control(regulation_on * p_ref * m_va_to_pu, regulation_on * p_meas);
            const auto iq_ref = -rst_outer_iq.control(regulation_on * q_ref, regulation_on * q_meas);

            //
            // Inner loop: dq-vector current control
            //
            // RST + 2 * ff for each loop
            const auto vd_ref = rst_inner_vd.control(-regulation_on * id_ref, regulation_on * id_meas) + vd_meas
                                + regulation_on * iq_meas * i_base * m_wl * m_si_to_pu;
            const auto vq_ref = rst_inner_vq.control(-regulation_on * iq_ref, regulation_on * iq_meas) + vq_meas
                                - regulation_on * id_meas * i_base * m_wl * m_si_to_pu;

            //
            // Frame conversion
            //
            const auto vd_ref_lim = limit.limit(vd_ref);
            const auto vq_ref_lim = limit.limit(vq_ref);

            const auto [v_a_ref, v_b_ref, v_c_ref] = dq0_to_abc.transform(vd_ref_lim, vq_ref_lim, 0.0, wt_pll);
            return std::make_tuple(v_a_ref, v_b_ref, v_c_ref);
        }

        // Owned Components
        vslib::SRFPLL                       pll;
        vslib::AbcToDq0Transform            abc_to_dq0_v;
        vslib::AbcToDq0Transform            abc_to_dq0_i;
        vslib::Dq0ToAbcTransform            dq0_to_abc;
        vslib::InstantaneousPowerThreePhase power_3ph_instant;
        vslib::RST<1>                       rst_outer_vdc;
        vslib::RST<2>                       rst_outer_id;
        vslib::RST<2>                       rst_outer_iq;
        vslib::RST<1>                       rst_inner_vd;
        vslib::RST<1>                       rst_inner_vq;
        vslib::LimitRange<double>           limit;

        // Owned Parameters
        vslib::Parameter<double> inductance;   //!< Inductance of the system [H]
        vslib::Parameter<double> frequency;    //!< Current frequency [Hz]
        vslib::Parameter<double> v_base;       //!< Base voltage [V]
        vslib::Parameter<double> i_base;       //!< Base current [A]

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_wl = 2.0 * std::numbers::pi * frequency.toValidate() * inductance.toValidate();

            // conversion constants, based on base voltage and base current:
            m_si_to_pu = sqrt(3.0 / 2.0) / v_base.toValidate();
            m_i_to_pu  = 1.0 / i_base.toValidate();
            m_va_to_pu = sqrt(2.0 / 3.0) * m_i_to_pu / v_base.toValidate();

            m_pu_to_v = 1.0 / m_si_to_pu;

            return {};
        }

      private:
        double m_wl{0.0};
        double m_si_to_pu{0.0};
        double m_pu_to_v{0.0};
        double m_i_to_pu{0.0};
        double m_va_to_pu{0.0};

        double m_delayed_start2{0.0};
        double m_delayed_start{0.0};
    };
}   // namespace user