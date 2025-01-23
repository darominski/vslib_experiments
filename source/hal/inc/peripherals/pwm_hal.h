//! @file
//! @brief Defines the hardware abstraction layer for a Pulse Width Modulation IP core.
//! @author Dominik Arominski

#pragma once

#include "cheby_gen/pwm_regs.h"

namespace hal
{

    class PWM
    {
      public:
        typedef myModule::PwmRegs::LoadMode UpdateType;

        //! Constructor for PWM HAL object.
        //!
        //! @param base_address Base address of the PWM IP, to be replaced by memory mapping.
        PWM(uint8_t* base_address)
        noexcept
            : m_regs(base_address)
        {
            // Assumption is made t hat the configuration does not change at runtime, so configuration can be
            // internalized in memory rather than always read from register
            m_max_counter_value = m_regs.ctrh.read();
        }

        //! Sets the desired modulation index.
        //!
        //! @param index Modulation index to be used, limited from -1 to 1
        void setModulationIndex(const float modulation_index) noexcept
        {
            const float index     = forceLimit(modulation_index, -1.0, 1.0);
            const float threshold = getMaximumCounterValue() * (0.5 * (index + 1));
            writeTriggerValue(static_cast<uint32_t>(threshold));
        }

        //! Configures the desired duty cycle for the PWM.
        //!
        //! @param fraction Duty cycle fraction percentage, 0-1.
        void setDutyCycle(const float duty_cycle) noexcept
        {
            const float fraction  = forceLimit(duty_cycle, 0.0, 1.0);
            const float threshold = getMaximumCounterValue() * (1.0 - fraction);
            writeTriggerValue(static_cast<uint32_t>(threshold));
        }

        //! Configures the PWM to stay high.
        void setHigh() noexcept
        {
            writeTriggerValue(0);
        }

        //! Configures the PWM to stay low.
        void setLow() noexcept
        {
            writeTriggerValue(getMaximumCounterValue());
        }

        //! Sets the update type.
        //!
        //! @param type Update type to be set, one of zero, prd, zeroPrd, immediate
        void setUpdateType(const UpdateType type) noexcept
        {
            m_regs.conf.loadMode.set(type);
        }

        //! Sets the additional extended dead time.
        //!
        //! @param additional_dead_time Additional dead time, in clock ticks
        void setExtendedDeadTime(const uint32_t additional_dead_time) noexcept
        {
            m_regs.extDeadtime.write(additional_dead_time);
        }

        //! Sets the disabled status for PWMA.
        //!
        //! @param setting Flag for setting the PWMA enabled status: disabled if true, enabled otherwise
        void setDisableA(const bool setting) noexcept
        {
            m_regs.dtctrl.disableA.set(false);
        }

        //! Sets the disabled status for PWMB.
        //!
        //! @param setting Flag for setting the PWMB enabled status: disabled if true, enabled otherwise
        void setDisableB(const bool setting) noexcept
        {
            m_regs.dtctrl.disableB.set(false);
        }

        //! Sets the inverted status of PWMA.
        //!
        //! @param setting Flag for setting the PWMA inverted status: inverted if true, not inverted otherwise
        void setInvertA(bool setting) noexcept
        {
            m_regs.dtctrl.inva.set(setting);
        }

        //! Sets the inverted status of PWMB.
        //!
        //! @param setting Flag for setting the PWMB inverted status: inverted if true, not inverted otherwise
        void setInvertB(bool setting) noexcept
        {
            m_regs.dtctrl.invb.set(setting);
        }

        void setEnable(const bool setting) noexcept
        {
            m_regs.ctrl.en.set(setting);
        }

        void reset(const bool setting) noexcept
        {
            m_regs.ctrl.reset.set(setting);
        }

        //! Reads the maximum counter value.
        //!
        //! @return Maximum counter value in clock ticks
        uint32_t getMaximumCounterValue() const noexcept
        {
            return m_max_counter_value;
        }

      private:
        myModule::PwmRegs m_regs;

        uint32_t m_max_counter_value{
            0};   //!< Maximum counter value to which the PWM counter is counting to, a configuration parameter.

        //! Writes to the PWM trigger register, allows to set the trigger for PWM to go high.
        //!
        //! @param threshold Threshold, in clock ticks, for PWM to go high
        void writeTriggerValue(uint32_t threshold) noexcept
        {
            m_regs.cc.write(threshold);
        }

        //! Utility method forcing the provided value to fit in the specified limits min and max.
        //!
        //! @param value
        //! @param limit_min
        //! @param limit_max
        //! @return Value fitting in the specified limits
        float forceLimit(const float value, const float limit_min, const float limit_max) const noexcept
        {
            if (value < limit_min)
            {
                return limit_min;
            }
            else if (value > limit_max)
            {
                return limit_max;
            }
            return value;
        }
    };
}