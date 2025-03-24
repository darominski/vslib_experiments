//! @file
//! @brief Class definiting the Component interface of PID controller.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <numbers>
#include <string>

#include "component.hpp"
#include "limitRange.hpp"
#include "parameter.hpp"
#include "rstController.hpp"

namespace vslib
{

    class PID : public Component
    {
        static constexpr unsigned int buffer_length = 3;   // length of the R, S, and T coefficients and history buffers

      public:
        //! PID controller constructor. Allows to set the name of the Component, set this controller as a child of a
        //! given parent Component, and specify an anti-windup function, which by default does not do anything.
        //!
        //! @param name Name identification of the PID controller
        //! @param parent Parent of this controller
        PID(std::string_view name, Component& parent)
            : Component("PID", name, parent),
              kp(*this, "kp"),                            // default limits
              ki(*this, "ki"),                            // default limits
              kd(*this, "kd"),                            // default limits
              kff(*this, "kff"),                          // default limits
              b(*this, "proportional_scaling"),           // default limits
              c(*this, "derivative_scaling"),             // default limits
              N(*this, "derivative_filter_order", 0.0),   // min limit: 0
              T(*this, "control_period", 0.0),            // min limit: 0.0
              f0(*this, "pre_warping_frequency", 0.0),    // min limit: 0.0
              actuation_limits("actuation_limits", *this),
              rst(name)
        {
        }

        //! Updates histories of measurements and references and moves the head of the history buffer.
        //!
        //! @param reference Current value of the set-point reference
        //! @param measurement Current value of the process value
        void updateInputHistories(const double reference, const double measurement) noexcept
        {
            rst.updateInputHistories(reference, measurement);
        }

        //! Computes one iteration of the controller.
        //!
        //! @param reference Reference value for the controller
        //! @param measurement Value of the controlled process
        //! @return Result of this iteration
        [[nodiscard]] double control(const double reference, const double measurement) noexcept
        {
            const double actuation         = rst.control(reference, measurement);
            const double clipped_actuation = actuation_limits.limit(actuation);
            if (clipped_actuation != actuation)
            {
                updateReference(clipped_actuation);
            }
            return clipped_actuation;
        }

        //! Updates the most recent reference in the history, used in cases actuation goes over the limit.
        //!
        //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
        void updateReference(const double updated_actuation)
        {
            rst.updateReference(updated_actuation);
        }

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept
        {
            rst.reset();
        }

        // ************************************************************
        // Getters

        //! Returns the r polynomial coefficients.
        //!
        //! @return r polynomial coefficients
        [[nodiscard]] const auto& getR() const
        {
            return rst.getR();
        }

        //! Returns the s polynomial coefficients.
        //!
        //! @return s polynomial coefficients
        [[nodiscard]] const auto& getS() const
        {
            return rst.getS();
        }

        //! Returns the t polynomial coefficients.
        //!
        //! @return t polynomial coefficients
        [[nodiscard]] const auto& getT() const
        {
            return rst.getT();
        }

        // ************************************************************
        // Settable coefficients of the controller

        Parameter<double> kp;    //!< Proportional gain coefficient
        Parameter<double> ki;    //!< Integral gain coefficient
        Parameter<double> kd;    //!< Derivative gain coefficient
        Parameter<double> kff;   //!< Feed-forward scaling coefficient
        Parameter<double> b;     //!< Reference signal proportional gain scaling (from DSP regFGC3)
        Parameter<double> c;     //!< Reference signal derivative gain scaling (from High-Performance Digital Control)
        Parameter<double> N;     //!< Filter order for derivative input
        Parameter<double> T;     //!< Control period
        Parameter<double> f0;    //!< Pre-warping frequency

        // ************************************************************
        // Limits of the controller's actuation

        LimitRange<double> actuation_limits;   //!< Range limiting of the actuation output

        // ************************************************************

        //! Update parameters method, called after any Parameter of this Component is modified.
        //!
        //! @return Optionally returns a Warning with pertinent information if verification was unsuccessful, nothing
        //! otherwise
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_1dof = false;   // reset the flag for 1 and 2 DOF choice
            // recalculation of PID interface into internal RST parameters, then: stability test

            const double k_p  = kp.toValidate();
            const double k_i  = ki.toValidate();
            const double k_d  = kd.toValidate();
            const double k_ff = kff.toValidate();
            const double b_   = b.toValidate();
            const double c_   = c.toValidate();
            const double N_   = N.toValidate();
            const double T_   = T.toValidate();
            const double f_0  = f0.toValidate();

            const double a  = 2.0 * std::numbers::pi_v<double> * f_0 / tan(std::numbers::pi_v<double> * f_0 * T_);
            const double a2 = pow(a, 2);   // helper a^2, which occurs often in the calculations below

            if (a == 0 || std::isnan(a))
            {
                return fgc4::utils::Warning(
                    fmt::format("Incorrect inputs to calculate RST coefficients: f_0: {}, T: {}.\n", f_0, T_)
                );
            }

            if (k_d == 0.0)
            {
                // there is no derivative element, N is assumed to be 1.0
                if (k_p != 0 && k_ff == 0.0 && b_ == 1)
                {
                    // 1DOF PI
                    m_r[0] = (k_p * k_i * (1 + k_p * a / k_i)) / a2;
                    m_r[1] = (k_p * k_i * (1 - k_p * a / k_i)) / a2;

                    m_s[0] = k_p / a;
                    m_s[1] = -k_p / a;

                    m_t[0] = m_r[0];
                    m_t[1] = m_r[1];
                    m_1dof = true;
                }
                else if (k_p == 0.0)
                {
                    // 1DOF I: simple integrator
                    m_r[0] = k_i / a;
                    m_r[1] = k_i / a;

                    m_s[0] = 1.0;
                    m_s[1] = -1.0;

                    m_t[0] = k_i / a + k_ff;
                    m_t[1] = k_i / a - k_ff;
                    m_1dof = true;
                }

                if (m_1dof)
                {
                    // for all 1st order controllers, the last elements are 0.0
                    m_r[2] = 0;
                    m_s[2] = 0;
                    m_t[2] = 0;
                }
            }
            if (!m_1dof && (k_p != 0.0 || k_d != 0))
            {
                const double ki_kp_N = k_i * k_p * N_;

                m_r[0] = (ki_kp_N + k_d * k_i * a + k_d * k_p * a2 + pow(k_p, 2) * N_ * a + k_d * k_p * N_ * a2) / a2;
                m_r[1] = 2.0 * (ki_kp_N - k_d * k_p * a2 - k_d * k_p * N_ * a2) / a2;
                m_r[2] = (ki_kp_N - k_d * k_i * a + k_d * k_p * a2 - pow(k_p, 2) * N_ * a + k_d * k_p * N_ * a2) / a2;

                m_s[0] = (k_d * a2 + k_p * N_ * a) / a2;
                m_s[1] = -2.0 * k_d;
                m_s[2] = (k_d * a2 - k_p * N_ * a) / a2;

                m_t[0] = (ki_kp_N + k_d * k_i * a + k_d * k_ff * a2 + k_d * k_p * a2 * b_ + pow(k_p, 2) * N_ * a * b_
                          + k_ff * k_p * N_ * a + k_d * k_p * N_ * a2 * c_)
                         / a2;
                m_t[1] = 2 * (ki_kp_N - k_d * k_ff * a2 - k_d * k_p * a2 * b_ - k_d * k_p * N_ * a2 * c_) / a2;
                m_t[2] = (ki_kp_N - k_d * k_i * a + k_d * k_ff * a2 + k_d * k_p * a2 * b_ - pow(k_p, 2) * N_ * a * b_
                          - k_ff * k_p * N_ * a + k_d * k_p * N_ * a2 * c_)
                         / a2;
            }

            // Jury's stability test, based on logic implemented in CCLIBS regRst.c
            if (m_r[0] == 0)
            {
                return fgc4::utils::Warning("First element of r coefficients is zero.");
            }

            if (m_s[0] == 0)
            {
                return fgc4::utils::Warning("First element of s coefficients is zero.");
            }

            if (m_t[0] == 0)
            {
                return fgc4::utils::Warning("First element of t coefficients is zero.");
            }

            const auto& warning_s = rst.jurysStabilityTest(m_s, 's');
            if (warning_s.has_value())
            {
                return warning_s.value();
            }

            const auto& warning_t = rst.jurysStabilityTest(m_t, 't');
            if (warning_t.has_value())
            {
                return warning_t.value();
            }

            // All tests passed, the calculated parameters can be forwarded to the RST
            rst.setR(m_r);
            rst.setS(m_s);
            rst.setT(m_t);

            return {};
        }

      private:
        std::array<double, buffer_length> m_r{0};   //!< Array holding a local copy of R coefficients
        std::array<double, buffer_length> m_s{0};   //!< Array holding a local copy of S coefficients
        std::array<double, buffer_length> m_t{0};   //!< Array holding a local copy of T coefficients

        RSTController<buffer_length> rst;   //!< RST controller responsible for the control logic

        bool m_1dof{false};   //!< Flag to define whether the controller has 1 (true) or 2 degrees of freedom
    };
}   // namespace vslib
