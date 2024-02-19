//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <numbers>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{

    class PIDRST : public Component
    {
        static constexpr unsigned int buffer_length = 3;   // length of the R, S, and T coefficients and history buffers

      public:
        //! PID controller constructor. Allows to set the name of the Component, set this controller as a child of a
        //! given parent Component, and specify an anti-windup function, which by default does not do anything.
        //!
        //! @param name Name identification of the PID controller
        //! @param parent Optional parent of this controller
        //! @return PID controller object
        PIDRST(std::string_view name, Component* parent = nullptr)
            : Component("PID", name, parent),
              kp(*this, "p", -10.0, 10.0),   // min limit: -10, max limit: 10
              ki(*this, "i", -10.0, 10.0),
              kd(*this, "d"),                       // default limits apply here
              kff(*this, "ff"),                     // default limits
              b(*this, "proportional_scaling"),     // default limits
              c(*this, "derivative_scaling"),       // default limits
              N(*this, "derivative_filter_order")   // default limits
        {
        }

        //! Updates histories of measurements and references and moves the head of the history buffer
        //!
        //! @param measurement Current value of the process value
        //! @param reference Current value of the set-point reference
        void update_input_histories(double measurement, double reference) noexcept
        {
            m_measurements[m_head] = measurement;
            m_references[m_head]   = reference;

            m_head++;
            if (m_head >= buffer_length)
            {
                m_history_ready = true;
                m_head          -= buffer_length;
            }
        }

        //! Computes one iteration of the controller
        //!
        //! @param process_value Value of the controlled
        //! @return Result of this iteration
        double control(double process_value, double reference) noexcept
        {
            // based on logic in regRstCalcActRT from CCLIBS libreg regRst.c
            update_input_histories(process_value, reference);

            double actuation = m_t[0] * m_references[m_head - 1] - m_r[0] * m_measurements[m_head - 1];

            for (size_t index = 1; index < buffer_length; index++)
            {
                int64_t buffer_index = (m_head - 1 - index);
                if (buffer_index < 0)
                {
                    buffer_index += buffer_length;
                }

                actuation += m_t[index] * m_references[buffer_index] - m_r[index] * m_measurements[buffer_index]
                             - m_s[index] * m_actuations[buffer_index];
            }
            actuation /= m_s[0];

            m_actuations[m_head] = actuation;   // update reference, m_head or m_head - 1?

            return actuation;
        }

        //! Updates the most recent reference in the history, used in cases actuation goes over the limit
        //!
        //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
        void update_reference(double updated_actuation)
        {
            // based on logic of regRstCalcRefRT from CCLIBS libreg's regRst.c
            m_actuations[m_head] = updated_actuation;

            double reference = 0;
            for (size_t index = 0; index < buffer_length; index++)
            {
                int64_t buffer_index = (m_head - 1 - index);
                if (buffer_index < 0)
                {
                    buffer_index += buffer_length;
                }

                reference += m_t[index] * m_references[buffer_index] - m_r[index] * m_measurements[buffer_index]
                             - m_s[index] * m_actuations[buffer_index];
            }
            m_references[m_head] = reference;
        }

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept
        {
            std::fill(std::begin(m_measurements), std::end(m_measurements), 0);
            std::fill(std::begin(m_references), std::end(m_references), 0);
            std::fill(std::begin(m_actuations), std::end(m_actuations), 0);
            m_head          = 0;
            m_history_ready = false;
        }

        //! Returns flag whether the reference and measurement histories are filled and RST is ready to regulate
        //!
        //! @return True if reference and measurement histories are filled, false otherwise
        bool isReady() const noexcept
        {
            return m_history_ready;
        }

        // ************************************************************
        // Settable coefficients of the controller

        Parameter<double> kp;    //!< Proportional gain coefficient
        Parameter<double> ki;    //!< Integral gain coefficient
        Parameter<double> kd;    //!< Derivative gain coefficient
        Parameter<double> kff;   //!< Feed-forward scaling coefficient
        Parameter<double> b;     //!< Reference signal proportional gain scaling (from DSP regFGC3)
        Parameter<double> c;     //!< Reference signal derivative gain scaling (from High-Performance Digital Control)
        Parameter<size_t> N;     //!< Filter order for derivative input

        //! Update parameters method, called after paramaters of this component are modified
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            // recalculation of PID interface into internal RST parameters, then: stability test
            // undefined variables: b, c, f0 - frequency, t_s - sampling period = vloop iteration period?, N -
            // derivative approximation?
            double f0;    // 300 kHz?, will be a settable parameter of STG
            double t_s;   // 1/T / f_b in [10, 25], f_b - bandwith of the closed-loop system

            double const kikpN = ki * kp * N;
            double const a     = 2.0 * std::numbers::pi_v<double> * f0 / atan(std::numbers::pi_v<double> * f0 * t_s);
            double const a2    = pow(a, 2);   // helper a^2, which occurs often in the calculations below

            m_r[0] = (kikpN + ki * kd * a + kd * kp * a2 + pow(kp, 2) * N * a + kd * kp * N * a2) / (4 * kikpN);
            m_r[1] = (2 * kikpN - 2 * kd * kp * a2 * (1 + N)) / (4 * kikpN);
            m_r[2] = (kikpN - kd * ki * a + kd * kp * a2 - pow(kp, 2) * N * a + kd * kp * N * a2) / (4 * kikpN);

            m_s[0] = (kd * a2 + kp * N * a) / (4 * kikpN);
            m_s[1] = (-kd * a2) / (2 * kikpN);
            m_s[2] = (kd * a2 - kp * N * a) / (4 * kikpN);

            m_t[0] = (kikpN + kd * ki * a + kd * kff * a2 + kd * kp * a2 * b + pow(kp, 2) * N * a * b + kff * kp * N * a
                      + kd * kp * N * a2 * c)
                     / (4 * kikpN);
            m_t[1] = (kikpN - kd * kff * a2 - kd * kp * a2 * b - kd * kp * N * a2 * c) / (2 * kikpN);
            m_t[2] = (kikpN - kd * ki * a + kd * kff * a2 + kd * kp * a2 * b - pow(kp, 2) * N * a * b - kff * kp * N * a
                      + kd * kp * N * a2 * c)
                     / (4 * kikpN);

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

            const auto& maybe_warning_s = jurysStabilityTest(m_r);
            if (maybe_warning_s.has_value())
            {
                return maybe_warning_s.value();
            }

            const auto& maybe_warning_t = jurysStabilityTest(m_t);
            if (maybe_warning_t.has_value())
            {
                return maybe_warning_t.value();
            }

            return {};
        }

      private:
        int64_t m_head{0};   //!< Index to latest entry in the history

        std::array<double, buffer_length> m_measurements;   //!< measurement history
        std::array<double, buffer_length> m_references;     //!< reference history
        std::array<double, buffer_length> m_actuations;     //!< actuation history

        std::array<double, buffer_length> m_r;   //!< r coefficients
        std::array<double, buffer_length> m_s;   //!< s coefficients
        std::array<double, buffer_length> m_t;   //!< t coefficients

        bool m_history_ready{false};   // flag to mark RST ref and meas histories are filled

        std::optional<fgc4::utils::Warning> jurysStabilityTest(const std::array<double, 3>& coefficients) const
        {
            int64_t coefficient_length = 1;
            while (coefficient_length < buffer_length && coefficients[coefficient_length] != 0.0F)
            {
                coefficient_length++;
            }
            coefficient_length--;

            double sum_even{0};
            double sum_odd{0};
            double sum_abs{0};

            for (size_t index = 0; index <= coefficient_length; index++)
            {
                const double coefficient = coefficients[index];

                sum_abs += abs(coefficient);

                if ((index & 1) == 0)
                {
                    sum_even += coefficient;
                }
                else
                {
                    sum_odd += coefficient;
                }
            }

            // Stability check 1 : Sum(even coefficients) >= Sum(odd coefficients)
            if (sum_even < sum_odd)
            {
                return fgc4::utils::Warning(
                    "RST unstable: sum of even coefficients less or equal than of odd coefficients.\n"
                );
            }

            // Stability check 2 : Sum(coefficients) > 0 - allow for floating point rounding errors
            if (((sum_even + sum_odd) / sum_abs) < -fgc4::utils::constants::floating_point_min_threshold)
            {
                return fgc4::utils::Warning(
                    "RST unstable: sum of coefficients below minimal floating-point threshold.\n"
                );
            }

            // Stability check 3 : Jury's Stability Test for unstable roots

            std::array<double, buffer_length> b = coefficients;
            std::array<double, buffer_length> a{0};

            while (coefficient_length > 2)
            {
                for (size_t index = 0; index <= coefficient_length; index++)
                {
                    a[index] = b[index];
                }

                double const d = a[coefficient_length] / a[0];

                for (size_t index = 0; index < coefficient_length; index++)
                {
                    b[index] = a[index] - d * a[coefficient_length - index];
                }

                // First element of every row of Jury's array > 0 for stability
                if (b[0] <= 0.0F)
                {
                    return fgc4::utils::Warning("RST unstable: the first element of Jury's array is not above zero.\n");
                }

                coefficient_length--;
            }

            // No warning returned because coefficients are stable
            return {};
        }
    };
}   // namespace vslib
