//! @file
//! @brief Class definiting two-degrees-of-freedom controller (RST).
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    template<size_t ControllerLength>
    class RST : public Component
    {
      public:
        RST(std::string_view name, Component* parent = nullptr)
            : Component("RST", name, parent),
              r(*this, "r", -12.0, 10.0),
              s(*this, "s", -12.0, 10.0),
              t(*this, "t", -12.0, 10.0),
        {
        }

        //! Calculates one iteration of the controller algorithm
        //!
        //! @param process_value Current process value (measurement)
        //! @param reference Reference value for the controller
        //! @return Controller output of the iteration
        double calculate_actuation(double process_value, double reference) noexcept
        {
            // based on logic in regRstCalcActRT from CCLIBS libreg regRst.c
            m_measurements[m_head] = process_value;
            m_references[m_head]   = reference;
            m_head++;
            if (m_head >= ControllerLength)
            {
                m_head -= ControllerLength;
            }

            double actuation = t[0] * m_reference[m_head - 1] - r[0] * m_measurements[m_head - 1];
            for (size_t index = 1; index < ControllerLength; index++)
            {
                const int64_t buffer_index = (m_head - 1 - index);
                if (buffer_index < 0)
                {
                    buffer_index += BufferLength;
                }

                actuation += t[index] * m_reference[buffer_index] - r[index] * m_measurements[buffer_index]
                             - s[index] * m_actuations[buffer_index];
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
                const int64_t buffer_index = (m_head - 1 - index);
                if (buffer_index < 0)
                {
                    buffer_index += BufferLength;
                }

                reference += t[index] * m_reference[buffer_index] - r[index] * m_measurements[buffer_index]
                             - s[index] * m_actuations[buffer_index];
            }
            m_reference[m_head] = reference;
        }

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept
        {
            m_measurements = std::array<double, ControllerLength>{0};
            m_references   = std::array<double, ControllerLength>{0};
            m_actuations   = std::array<double, ControllerLength>{0};
        }

        // ************************************************************
        // Settable Parameters

        Parameter<std::array<double, ControllerLength>> r;   //<! reference coefficients
        Parameter<std::array<double, ControllerLength>> s;   //<! disturbance coefficients
        Parameter<std::array<double, ControllerLength>> t;   //<! control coefficients

      private:
        int64_t                              m_head{0};        // Index to latest entry in the history
        std::array<double, ControllerLength> m_measurements;   // RST measurement history
        std::array<double, ControllerLength> m_references;     // RST reference history
        std::array<double, ControllerLength> m_actuations;     // RST actuation history.
    };
}   // namespace vslib
