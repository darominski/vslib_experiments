//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <numbers>
#include <string>

#include "component.h"
#include "parameter.h"
#include "rstController.h"

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
              kp(*this, "p", -10.0, 10.0),              // min limit: -10, max limit: 10
              ki(*this, "i", -10.0, 10.0),              // min limit: -10, max limit: 10
              kd(*this, "d"),                           // default limits apply here
              kff(*this, "ff"),                         // default limits
              b(*this, "proportional_scaling"),         // default limits
              c(*this, "derivative_scaling"),           // default limits
              N(*this, "derivative_filter_order", 0),   // min limit: 0
              ts(*this, "sampling_period", 0.0),        // min limit: 0.0
              f0(*this, "control_frequency", 0.0)       // min limit: 0.0
        {
        }

        //! Updates histories of measurements and references and moves the head of the history buffer
        //!
        //! @param measurement Current value of the process value
        //! @param reference Current value of the set-point reference
        void update_input_histories(double measurement, double reference) noexcept
        {
            rst.update_input_histories(measurement, reference);
        }

        //! Computes one iteration of the controller
        //!
        //! @param process_value Value of the controlled
        //! @return Result of this iteration
        double control(double process_value, double reference) noexcept
        {
            return rst.control(process_value, reference);
        }

        //! Updates the most recent reference in the history, used in cases actuation goes over the limit
        //!
        //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
        void update_reference(double updated_actuation)
        {
            rst.update_reference(updated_actuation);
        }

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept
        {
            rst.reset();
        }

        // ************************************************************
        // Getters

        //! Returns flag whether the reference and measurement histories are filled and RST is ready to regulate
        //!
        //! @return True if reference and measurement histories are filled, false otherwise
        bool isReady() const noexcept
        {
            return rst.isReady();
        }

        //! Returns the r polynomial coefficients
        //!
        //! @return r polynomial coefficients
        const auto& getR() const
        {
            return rst.getR();
        }

        //! Returns the s polynomial coefficients
        //!
        //! @return s polynomial coefficients
        const auto& getS() const
        {
            return rst.getS();
        }

        //! Returns the t polynomial coefficients
        //!
        //! @return t polynomial coefficients
        const auto& getT() const
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
        Parameter<double> ts;    //!< Sampling period
        Parameter<double> f0;    //!< Control freqency
        Parameter<size_t> N;     //!< Filter order for derivative input

        //! Update parameters method, called after parameters of this component are modified
        //!
        //! @return Optionally returns a Warning with pertinent information if verification was unsuccessful, nothing
        //! otherwise
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            // recalculation of PID interface into internal RST parameters, then: stability test

            double const kikpN = ki * kp * N;
            double const a     = 2.0 * std::numbers::pi_v<double> * f0 / atan(std::numbers::pi_v<double> * f0 * ts);
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

            const auto& maybe_warning_s = rst.jurysStabilityTest(m_r);
            if (maybe_warning_s.has_value())
            {
                return maybe_warning_s.value();
            }

            const auto& maybe_warning_t = rst.jurysStabilityTest(m_t);
            if (maybe_warning_t.has_value())
            {
                return maybe_warning_t.value();
            }

            // All tests passed, the calculated parameters can be forwarded to the RST
            rst.setR(m_r);
            rst.setS(m_s);
            rst.setT(m_t);

            return {};
        }

      private:
        std::array<double, 3> m_r;
        std::array<double, 3> m_s;
        std::array<double, 3> m_t;

        RSTController<3> rst;
    };
}   // namespace vslib
