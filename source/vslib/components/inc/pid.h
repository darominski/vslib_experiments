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
              kp(*this, "p", -10.0, 10.0),   // min limit: -10, max limit: 10
              ki(*this, "i", -10.0, 10.0),
              kd(*this, "d"),   // default limits apply here
              integral_limit(*this, "integral_limit"),
              m_anti_windup_protection(anti_windup_protection)
        {
        }

        //! Computes one iteration of the controller
        //!
        //! @param current_value Value of the controlled
        //! @return Result of this iteration
        double control(double current_value)
        {
            m_error = m_target - current_value;
            processIntegralError(m_error);
            double const derivative = m_error - m_previous_error;   // assuming time difference denominator is = 1
            double const output     = kp * m_error + ki * m_integral + kd * derivative;
            m_previous_error        = m_error;   // update errors for the next iteration
            return output;
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
            m_target         = 0;
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

        //! Returns the target value of the controller
        //!
        //! @return Target value of the controller
        [[nodiscard]] double getTarget() const noexcept
        {
            return m_target;
        }

        //! Sets the starting value of the controller
        //!
        //! @param value Starting value for the controller
        void setStartingValue(double value) noexcept
        {
            m_starting_value = value;
        }

        //! Sets the target value of the controller
        //!
        //! @param target Target value for the controller
        void setTarget(double target) noexcept
        {
            m_target = target;
        }

        // ************************************************************
        // Settable coefficients of the controller

        Parameter<double> kp;   //!< Proportional gain coefficient
        Parameter<double> ki;   //!< Integral gain coefficient
        Parameter<double> kd;   //!< Derivative gain coefficient

        Parameter<double> integral_limit;   //!< limit of the integral error

      private:
        double m_starting_value{0};   //!< Starting value of control
        double m_target{0};           //!< Target setpoint
        double m_error{0};            //!< Current error value
        double m_previous_error{0};   //!< Previous error value
        double m_integral{0};         //!< Cumulative error over time

        std::function<double(double)> m_anti_windup_protection;

        //! Accumulates the error and calls the anti-windup function
        //!
        //! @param error Current control iteration error
        void processIntegralError(double error) noexcept
        {
            m_integral += error;
            m_integral = m_anti_windup_protection(m_integral);
        }
    };
}   // namespace vslib
