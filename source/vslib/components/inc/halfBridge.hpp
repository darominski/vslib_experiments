//! @file
//! @brief Component implementing an interface for the Pulse width modulation IP core.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.hpp"
#include "parameter.hpp"
#include "peripherals/pwm_hal.hpp"

namespace vslib
{
    class HalfBridge : public Component
    {
      public:
        HalfBridge(std::string_view name, Component& parent, uint8_t* base_address) noexcept
            : Component("HalfBridge", name, parent),
              m_pwm(base_address)
        {
            // initialize the right PWM IP out of the list of 12, TODO
            // right now, base_address is needed
        }

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

        void reset() noexcept
        {
            m_pwm.reset();
        }

        //! Sets the duty cycle of the PWM. When set to 0.0 (%), the PWM is held low, when set to 100 (%), the PWM is
        //! held high.
        //!
        //! @param duty_cycle Value between 0.0 and 1.0 for the duty cycle of the PWM
        void setDutyCycle(const float duty_cycle) noexcept
        {
            m_pwm.setModulationIndex(duty_cycle);
        }

        //! Sets the modulation index.
        //!
        //! @param index Modulation index, -1 to 1
        void setModulationIndex(const float index) noexcept
        {
            // force limit of -1.0, 1.0
            m_pwm.setModulationIndex(index);
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
            m_pwm.setInvert(setting);
        }

        // ************************************************************
        // Owned Parameters


        // ************************************************************
        // Owned Components

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            return {};
        }

        static auto constexpr size() noexcept
        {
            return hal::PWM::size();
        }

      private:
        hal::PWM m_pwm;   //!< PWM IP core HAL
    };
}