//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    class PID : public Component
    {
      public:
        //! PID controller constructor. Allows to set the name of the Component, set this controller as a child of a
        //! given parent Component, and specify an anti-windup function, which by default does not do anything.
        //!
        //! @param name Name identification of the PID controller
        //! @param parent Optional parent of this controller
        //! @param anti_windup_protection Optional anti-windup protection method. It needs to take double (integral
        //! value) and return a double (corrected integral value)
        //! @return PID controller object
        PID(
            std::string_view name, Component* parent = nullptr,
            std::function<double(double)> anti_windup_protection =
                [](double input)
            {
                return input;
            }
        )
            : Component("PID", name, parent),
              kp(*this, "kp", -10.0, 10.0),   // min limit: -10, max limit: 10
              ki(*this, "ki", -10.0, 10.0),
              kd(*this, "kd"),     // default limits apply here
              kff(*this, "kff"),   // default limits apply here
              b(*this, "b"),
              c(*this, "c"),
              integral_limit(*this, "integral_limit"),
              m_anti_windup_protection(anti_windup_protection)
        {
        }

        //! Computes one iteration of the controller
        //!
        //! @param process_value Value of the controlled process
        //! @param reference Value of the set-point reference
        //! @return Result of this iteration
        double control(double process_value, double reference)
        {
            m_error    = reference - process_value;
            m_integral += m_error;
            m_integral = m_anti_windup_protection(m_integral);

            double const feed_forward = kff * reference;
            double const proportional = kp * (reference * b - process_value);
            double const integral     = ki * m_integral;
            // assuming time difference denominator is included in kd:
            m_derivative              = -0.2 * m_derivative + kd * (m_error + 0.8 * m_previous_error);
            double const actuation    = feed_forward + proportional + integral + m_derivative;

            // update errors for the next iteration
            m_previous_error = m_error;

            return actuation;
        }

        //! Allows for resetting errors of the controller and setting new starting value
        //!
        //! @param start_value New starting value of the controller
        void reset(double start_value) noexcept
        {
            m_starting_value = start_value;
            m_error          = 0;
            m_integral       = 0;
            m_previous_error = 0;
        }

        // ************************************************************
        // Standard getters and setters

        //! Returns the target value of the controller
        //!
        //! @return Target value of the controller
        [[nodiscard]] double getError() const noexcept
        {
            return m_error;
        }

        //! Returns the target value of the controller
        //!
        //! @return Integral error value of the controller
        [[nodiscard]] double getIntegral() const noexcept
        {
            return m_integral;
        }

        //! Returns the target value of the controller
        //!
        //! @return Target value of the controller
        [[nodiscard]] double getPreviousError() const noexcept
        {
            return m_previous_error;
        }

        //! Returns the starting value of the controller
        //!
        //! @return Starting value of the controller
        [[nodiscard]] double getStartingValue() const noexcept
        {
            return m_starting_value;
        }

        //! Sets the starting value of the controller
        //!
        //! @param value Starting value for the controller
        void setStartingValue(double value) noexcept
        {
            m_starting_value = value;
        }

        // ************************************************************
        // Settable coefficients of the controller

        Parameter<double> kp;    //!< Proportional gain coefficient
        Parameter<double> ki;    //!< Integral gain coefficient
        Parameter<double> kd;    //!< Derivative gain coefficient
        Parameter<double> kff;   //!< Feed-forward scaling coefficient
        Parameter<double> b;     //!< Gain of the proportional stage reference signal
        Parameter<double> c;     //!< Gain of the proportional stage reference signal

        Parameter<double> integral_limit;   //!< limit of the integral error

      private:
        double m_starting_value{0};   //!< Starting value of control
        double m_error{0};            //!< Current error value
        double m_previous_error{0};   //!< Previous error value
        double m_integral{0};         //!< Cumulative error over time
        double m_derivative{0};       //!< Cumulative derivative part of actuation

        std::function<double(double)> m_anti_windup_protection;
    };
}   // namespace vslib
