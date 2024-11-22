//! @file
//! @brief Component implementing an interface for the Pulse width modulation IP core.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "parameter.h"
#include "peripheralInterrupt.h"

namespace vslib
{
    class PWM : public Component
    {
      public:
        PWM(std::string_view name, Component& parent, int physical_id, )
        noexcept
            : Component("PWM", name, parent),
              modulation_limits(*this, "modulation_limits"),
              additional_dead_time("additional_dead_time", *this, 0.0),
        {
            // initialize the right PWM IP out of the list of 12:
            m_pwm = utils::assign_ip_object("PwmIp", physical_id);
        }

        //! Sets the duty cycle of the PWM. When set to 0.0 (%), the PWM is held low, when set to 100 (%), the PWM is
        //! held high.
        //!
        //! @param duty_cycle Value between 0.0 and 100.0 for the duty cycle of the PWM
        void setDutyCycle(const double duty_cycle) noexcept
        {
            // force 0 - 100%
            // sets duty cycle, take into account the offset
        }

        //! @param index Modulation index, -1 to 1
        void setModulationIndex(const double index) noexcept
        {
            // if index=0.5, duty cycle = 50%
            // shift by offset m_phase_offset
            const double modulation_limits_double.limit(index);
        }

        //! @param counter Counter value, from 0 to CTRH
        void setModulationIndex(const uint32_t counter) noexcept
        {

            // check against m_limit_min_uint, m_limit_max_uint
        }

        void setHigh() noexcept
        {
        }

        void setLow() noexcept
        {
        }

        //! Allows to set the phase offset in lieu of shifting the PWM carrier.
        //!
        //! @param offset Offset to be set
        void setPhaseOffset(const double offset) noexcept
        {
            m_phase_offset = offset;
            // and modify the modulation index, shift CC0 and CC1
        }

        void setAdditionalDeadTime(const double dead_time) noexcept
        {
            // adds this to PWM IP register
            // m_pwm->extended_dead_time = dead_time;
        }

        void setUpdateType()
        {
            // ???
        }

        void invert(const bool inverter)
        {
            // at runtime? at configuration?
        }

        void start() noexcept
        {
            m_pwm->enable = 0xFFFF;
        }

        void stop() noexcept
        {
            m_pwm->enable = 0x0000;
        }

        // ************************************************************
        // Owned Components

        LimitRange<double> modulation_limits;   //!< Range limiting of the possible values for the modulation index

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_limit_min_uint = static_cast<uint32_t>(modulation_limits.min.toValidate() * (m_pwm->CTRH / 2.0));
            m_limit_max_uint = static_cast<uint32_t>(modulation_limits.max.toValidate() * (m_pwm->CTRH / 2.0));

            return {};
        }

      private:
        volatile PwmIp* m_pwm;   //!< PWM IP core export

        //! Phase offset, handles the offsetting the CC0, CC1 instead of shifting the carrier
        double m_phase_offset{0.0};

        uint32_t m_limit_min_uint;
        uint32_t m_limit_max_uint;
    };
}