//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    class PIDRST : public Component
    {
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
              kd(*this, "d"),   // default limits apply here
        {
        }

        //! Computes one iteration of the controller
        //!
        //! @param process_value Value of the controlled
        //! @return Result of this iteration
        double control(double process_value, double reference) noexcept
        {
            double actuation       = 0;
            // based on logic in regRstCalcActRT from CCLIBS libreg regRst.c
            m_measurements[m_head] = process_value;
            m_references[m_head]   = reference;

            m_head++;
            if (m_head >= ControllerLength)
            {
                m_head -= ControllerLength;
            }

            double actuation = m_t[0] * m_references[m_head - 1] - m_r[0] * m_measurements[m_head - 1];
            for (size_t index = 1; index < ControllerLength; index++)
            {
                int64_t buffer_index = (m_head - 1 - index);
                if (buffer_index < 0)
                {
                    buffer_index += ControllerLength;
                }

                actuation += m_t[index] * m_references[buffer_index] - m_r[index] * m_measurements[buffer_index]
                             - m_s[index] * m_actuations[buffer_index];
            }
            actuation /= s[0];

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
            for (size_t index = 0; index < ControllerLength; index++)
            {
                int64_t buffer_index = (m_head - 1 - index);
                if (buffer_index < 0)
                {
                    buffer_index += ControllerLength;
                }

                reference += t[index] * m_references[buffer_index] - r[index] * m_measurements[buffer_index]
                             - s[index] * m_actuations[buffer_index];
            }
            m_references[m_head] = reference;
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
            m_set_point      = 0;
        }

        // ************************************************************
        // Settable coefficients of the controller

        Parameter<double> kp;    //!< Proportional gain coefficient
        Parameter<double> ki;    //!< Integral gain coefficient
        Parameter<double> kd;    //!< Derivative gain coefficient
        Parameter<double> kff;   //!< Feed-forward scaling coefficient

        //! Update parameters method, called after paramaters of this component are modified
        void updateParameters()
        {
            // recalculation of PID interface into internal RST parameters, then: stability test
        }

      private:
        int64_t m_head{0};   //!< Index to latest entry in the history

        std::array<double, ControllerLength> m_measurements;   //!< measurement history
        std::array<double, ControllerLength> m_references;     //!< reference history
        std::array<double, ControllerLength> m_actuations;     //!< actuation history

        std::array<double, ControllerLength> m_r;   //!< r coefficients
        std::array<double, ControllerLength> m_s;   //!< s coefficients
        std::array<double, ControllerLength> m_t;   //!< t coefficients
    };
}   // namespace vslib
