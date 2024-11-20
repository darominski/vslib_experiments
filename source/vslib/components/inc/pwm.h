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
    class PulseWidthModulator : public Component
    {
      public:
        PulseWidthModulator(
            std::string_view name, Component& parent, int physical_id,
            std::function<void(PulseWidthModulator&)> callback
        ) noexcept
            : Component("PWM", name, parent),
              modulation_limits(*this, "modulation_limits") phase_offset("phase_offset", *this),
              additional_dead_time("additional_dead_time", *this, 0.0),
        {
            // initialize the right PWM IP out of the list of 12:
            m_pwm = utils::assign_ip_object("PwmIp", physical_id);
        }

        //! Sets the duty cycle of the PWM. When set to 0.0 (%), the PWM is held low, when set to 100 (%), the PWM is
        //! held high.
        //!
        //! @param duty_cycle Value between 0 and 100 for the duty cycle of the PWM
        void setDutyCycle(const double duty_cycle) noexcept
        {
            // sets duty cycle
        }

        //! Allows to set the phase offset in lieu of shifting the PWM carrier.
        //!
        //! @param offset Offset to be set
        void setPhaseOffset(const double offset) noexcept
        {
            m_phase_offset = offset;
        }

        void start() noexcept
        {
            m_pwm.stg.start();
        }

        void stop() noexcept
        {
            m_pwm.stg.stop();
        }

        // ************************************************************
        // Settable Parameters

        Parameter<double> additional_dead_time;

        // ************************************************************
        // Owned Components

        LimitRange<double> modulation_limits;   //!< Range limiting of the possible values for the modulation index

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            return {};
        }

      private:
        volatile PwmIp* m_pwm;   //!< PWM IP core export

        PeripheralInterrupt interrupt;   //!< Interrupt interface for the STG interrupt attached to the PWM

        double m_phase_offset{
            0.0};   //!< Phase offset, handles the offsetting the CC0, CC1 instead of shifting the carrier
    };
}