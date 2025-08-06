//! @file
//! @brief Component implementing an interface for the Pulse width modulation IP core.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.hpp"
#include "parameter.hpp"
#include "peripherals/pwm.hpp"

namespace vslib
{
    class HalfBridge : public Component
    {
      public:
        //! Constructor for the HalfBridge Component.
        //!
        //! @param name Name of this Component
        //! @param parent Parent of this Component
        //! @param pwm_id ID of the FPGA's PWM that this HalfBridge interacts with
        //! @param max_counter_value Maximal value of the PWM counter (half-period length)
        HalfBridge(
            std::string_view name, Component& parent, const uint32_t pwm_id, const uint32_t max_counter_value
        ) noexcept
            : Component("HalfBridge", name, parent),
              m_pwm(pwm_id, max_counter_value)
        {
        }

        // ************************************************************
        // General start/stop methods

        //! Starts the PWM IP by enabling the PWMA and PWMB.
        void start() noexcept
        {
            m_pwm.setEnabledA(true);
            m_pwm.setEnabledB(true);
            m_pwm.setEnabled(true);
        }

        //! Stops the PWM IP by disabling the PWMA and PWMB.
        void stop() noexcept
        {
            m_pwm.setEnabledA(false);
            m_pwm.setEnabledB(false);
            m_pwm.setEnabled(false);
        }

        // ************************************************************
        // Setters

        //! Sets the enabled status for PWMA.
        //!
        //! @param setting Flag for setting the PWMA enabled status: enabled if true, disabled otherwise
        void setEnabledA(const bool setting) noexcept
        {
            m_pwm.setEnabledA(setting);
        }

        //! Sets the enabled status for PWMB.
        //!
        //! @param setting Flag for setting the PWMA enabled status: enabled if true, disabled otherwise
        void setEnabledB(const bool setting) noexcept
        {
            m_pwm.setEnabledA(setting);
        }

        //! Sets the modulation index.
        //!
        //! @param index Modulation index, 0 to 1
        [[maybe_unused]] bool setModulationIndex(const float index) noexcept
        {
            // force limit of 0.0, 1.0
            return m_pwm.setModulationIndex(index);
        }

        //! Sets the output to high.
        void setHigh() noexcept
        {
            m_pwm.setHigh();
        }

        //! Sets the output to low.
        void setLow() noexcept
        {
            m_pwm.setLow();
        }

        //! Sets the additional dead time.
        //!
        //! @param dead_time Additional dead time in clock ticks
        void setAdditionalDeadTime(const uint32_t dead_time) noexcept
        {
            m_pwm.setExtendedDeadTime(dead_time);
        }

        //! Sets the update type.
        //!
        //! @param update_type Update type to be set, one of zero, prd, zero_prd, and immediate
        void setUpdateType(hal::PWM::UpdateType update_type) noexcept
        {
            m_pwm.setUpdateType(update_type);
        }

        //! Sets the inverted state of PWMA and PWMB to a desired setting.
        //!
        //! @param setting New setting for inversion of PWMA: inverted if true, not-inverted otherwise
        void setInverted(const bool setting) noexcept
        {
            m_pwm.setInverted(setting);
        }

        // ************************************************************
        // Owned Components

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            return {};
        }

      private:
        hal::PWM m_pwm;   //!< PWM IP core HAL
    };
}