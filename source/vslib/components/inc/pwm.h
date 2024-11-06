//! @file
//! @brief Component implementing an interface for the Pulse width modulation IP core.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    enum class CarrierType
    {
        sawtooth,
        triangular
    }

    class PulseWidthModulator : public Component
    {
      public:
        PulseWidthModulator(std::string_view name, Component& parent, int physical_id)
            : Component("PWM", name, parent),
              active_edge_limits(*this, "active_edge_limits") duty_cycle("duty_cycle", *this, 0, 100),
              period("period", *this, 0.0),
              frequency("frequency", *this, 0.0),
              resolution("resolution", *this),
              rise_time("rise_time", *this, 0.0),
              fall_time("fall_time", *this, 0.0),
              modulation_index("modulation_index", *this),
              phase_offset("phase_offset", *this),
              minimum_off_time("minimum_off_time", *this, 0.0),
              minimum_on_time("minimum_on_time", *this, 0.0),
              dead_time("dead_time", *this, 0.0),
              carrier_type("carrier_type", *this)
        {
            // initialize the right PWM IP out of the list of 12:
            m_pwm = utils::assign_ip_object("PwmIp", physical_id);
        }

        // ************************************************************
        // Settable Parameters
        Parameter<double>      duty_cycle;
        Parameter<double>      period;
        Parameter<double>      frequency;
        Parameter<double>      resolution;
        Parameter<double>      rise_time;
        Parameter<double>      fall_time;
        Parameter<double>      modulation_index;
        Parameter<double>      phase_offset;
        Parameter<double>      minimum_off_time;
        Parameter<double>      minimum_on_time;
        Parameter<double>      dead_time;
        Parameter<CarrierType> carrier_type;

        // ************************************************************
        // Owned Components

        LimitRange<double> active_edge_limits;   //!< Range limiting of the possible values for CC0 and CC1

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            // issue: frequency and period set the same variable, but we want to offer them as a convenience to users,
            // so they need to set only one. Since all Parameters need to be properly initialized, how to approach this?
            // ugly fix: use 0.0 in one of them as a signal of not being used
            if (period.toValidate() == 0.0 && frequency.toValidate() == 0.0)
            {
                return fgc4::utils::Warning("{}: both period and frequency set to zero.", m_name);
            }
            else if (period.toValidate() == 0.0)
            {
                m_pwm->frequency = (1.0 / period.toValidate());
            }
            else if (frequency.toValidate() == 0.0)
            {
                m_pwm->frequency = frequency.toValidate();
            }
            else
            {
                return fgc4::utils::Warning("{}: both period and frequency set to a plausible values, not possible to "
                                            "distinguish which should be the source of PWM frequency." m_name);
            }

            m_pwm->frequency = duty_cycle.toValidate();
            // etc.

            return {};
        }

      private:
        double m_frequency{0.0};   //!< PWM frequency

        volatile PwmIp* m_pwm;
    };
}