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

            const uint32_t dead_time = m_regs.deadtime.read();   // dead time, in clock cycles
            const uint32_t min_switch_time
                = m_regs.minSwitchTimeSc.read();   // minimum ON or OFF switch time, in clock cycles
            // max counter value serves as period in clock cycles
            m_max_modulation_index
                = static_cast<float>(2 * (m_max_counter_value - min_switch_time + dead_time) / m_max_counter_value + 1);
            m_min_modulation_index
                = -(2.0 * static_cast<float>((m_max_counter_value - min_switch_time - dead_time) / m_max_counter_value)
                    - 1.0);
            m_max_duty_cycle = 0.5 * (m_max_modulation_index - m_min_modulation_index);
        }

        //! Sets the desired modulation index.
        //!
        //! @param index Modulation index to be used, limited from -1 to 1 (at maximum)
        void setModulationIndex(const float modulation_index) noexcept
        {
            const float index     = forceLimit(modulation_index, m_min_modulation_index, m_max_modulation_index);
            const float threshold = getMaximumCounterValue() * (0.5 * (index + 1));
            writeTriggerValue(static_cast<uint32_t>(threshold));
        }

        //! Configures the desired duty cycle for the PWM.
        //!
        //! @param fraction Duty cycle fraction percentage, 0-1 (at maximum)
        void setDutyCycle(const float duty_cycle) noexcept
        {
            const float fraction  = forceLimit(duty_cycle, 0.0, m_max_duty_cycle);
            const float threshold = getMaximumCounterValue() * (m_max_modulation_index - fraction);
            writeTriggerValue(static_cast<uint32_t>(threshold));
        }

        //! Configures the PWM to stay high.
        void setHigh() noexcept
        {
            writeTriggerValue(0);   // or min_modulation_index?
        }

        //! Configures the PWM to stay low.
        void setLow() noexcept
        {
            writeTriggerValue(getMaximumCounterValue());   // or max_modulation_index?
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

        //! Sets the enabled status for PWMA.
        //!
        //! @param setting Flag for setting the PWMA enabled status: enabled if true, disabled otherwise
        void setEnableA(const bool setting) noexcept
        {
            m_regs.dtctrl.disableA.set(!setting);
        }

        //! Sets the enabled status for PWMB.
        //!
        //! @param setting Flag for setting the PWMB enabled status: enabled if true, disabled otherwise
        void setEnableB(const bool setting) noexcept
        {
            m_regs.dtctrl.disableB.set(!setting);
        }

        //! Sets the inverted status of PWMA.
        //!
        //! @param setting Flag for setting the PWMA inverted status: inverted if true, not inverted otherwise
        void setInvert(bool setting) noexcept
        {
            m_regs.dtctrl.inva.set(setting);
            m_regs.dtctrl.invb.set(setting);
        }

        //! Enables the PWM counters.
        //!
        //! @param setting Setting for the counters,
        void setEnable(const bool setting) noexcept
        {
            m_regs.ctrl.en.set(setting);
        }

        //! Reads the maximum counter value.
        //!
        //! @return Maximum counter value in clock ticks
        uint32_t getMaximumCounterValue() const noexcept
        {
            return m_max_counter_value;
        }

        static auto constexpr size() noexcept
        {
            return myModule::PwmRegs::csize();
        }

      private:
        myModule::PwmRegs m_regs;

        //! Maximum counter value to which the PWM counter is counting to, a configuration parameter.
        uint32_t m_max_counter_value{0};
        //! Minimum modulation index that can be set for this particular PWM.
        float m_min_modulation_index{-1.0};
        //! Maximum modulation index that can be set for this particular PWM.
        float m_max_modulation_index{1.0};
        //! Maximum duty cycle due to limits imposed on the modulation indices;
        float m_max_duty_cycle{1.0};

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