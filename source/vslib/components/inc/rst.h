#pragma once

#include <array>
#include <string>

#include "component.h"
#include "magic_enum.hpp"
#include "parameter.h"

namespace vslib
{
    // Helper enumeration class for definition of all relevant statuses
    enum class Status
    {
        uninitialized,
        ready,
        updating,
        fault
    };

    // ************************************************************

    template<size_t ControllerLength>
    class RST : public Component
    {
      public:
        RST(std::string_view name, Component* parent)
            : Component("RST", name, parent),
              r(*this, "r", -12.0, 10.0),
              s(*this, "s", -12.0, 10.0),
              t(*this, "t", -12.0, 10.0),
              status(*this, "status")
        {
        }

        //! Calculates one iteration of the controller algorithm
        //!
        //! @param reference Reference value for the controller
        //! @return Controller output of the iteration
        double control(double reference) noexcept
        {
            // Calculate the control output
            double const control_output = std::inner_product(r.cbegin(), r.cend(), m_current_state_r.cbegin(), 0.0);

            // Update the state vector
            std::rotate(m_current_state_r.rbegin(), m_current_state_r.rbegin() + 1, m_current_state_r.rend());
            m_current_state_r[0] = reference - control_output;

            // Calculate the tracking output
            double const tracking_output = std::inner_product(s.begin(), s.cend(), m_current_state_s.cbegin(), 0.0);

            // Update the state vector for the S filter
            std::rotate(m_current_state_s.rbegin(), m_current_state_s.rbegin() + 1, m_current_state_s.rend());
            m_current_state_s[0] = m_output - tracking_output;

            m_output = std::inner_product(s.cbegin(), s.cend(), m_current_state_s.cbegin(), 0.0)
                + std::inner_product(t.cbegin(), t.cend(), m_current_state_r.cbegin(), 0.0);
            return m_output;
        }

        //! Resets the controller to the initial state, zeroing all buffers
        void reset() noexcept
        {
            m_output = 0;
            for (size_t index = 0; index < ControllerLength; index++)
            {
                m_current_state_r[index] = 0;
                m_current_state_s[index] = 0;
            }
        }

        // ************************************************************
        // Getters

        //! Returns the control output value
        //!
        //! @return Control output value
        [[nodiscard]] double getOutput() const noexcept
        {
            return m_output;
        }

        //! Provides status value as a string
        //!
        //! @return String view of the status of this component
        [[nodiscard]] std::string_view getStatusAsStr() const
        {
            return magic_enum::enum_name(status.value());
        }

        // ************************************************************
        // Settable Parameters

        Parameter<std::array<double, ControllerLength>> r;   //<! reference coefficients
        Parameter<std::array<double, ControllerLength>> s;   //<! disturbance coefficients
        Parameter<std::array<double, ControllerLength>> t;   //<! control coefficients
        Parameter<Status>                               status;

      private:
        std::array<double, ControllerLength> m_current_state_r{0};
        std::array<double, ControllerLength> m_current_state_s{0};
        double                               m_output{0};
    };
}   // namespace vslib
