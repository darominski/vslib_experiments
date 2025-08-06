//! @file
//! @brief Defines the hardware abstraction layer for a Pulse Width Modulation IP core.
//! @author Dominik Arominski

#pragma once

#include "cheby_gen/mb_top_singleton.hpp"

namespace hal
{
    class PWM
    {
      public:
        typedef ipCores::Top::PwmArrayItem::Pwm::UpdateType UpdateType;

        //! Constructor for PWM HAL object.
        //!
        //! @param max_counter_value Maximal value of the PWM counter
        PWM(const uint32_t pwm_id, const uint32_t max_counter_value) noexcept
        {
            m_regs = hal::Top::instance().pwm[pwm_id].pwm;

            setConfiguration(max_counter_value, 2'000, 1'000);
            configureMinMaxModulation();
        }

        //! Configures the IP core in absence of the Configurator.
        //!
        //! @param max_counter_value Maximum value of the PWM counter (CTRH)
        //! @param dead_time Dead time between PWMA and PWMB
        //! @param min_switch_time Mininum switching off time
        //! @param update_type Update type, one out of immediate or shadowed modes: zero, period, or zero_period
        //! @param enable_pwm_check Switch to enable PWM minimum off time protection
        //! @param enable_shoot_through_check Switch to enable shoot-through protection
        //! @param bypass_dead_time Switch to bypass dead_time
        //! @param enable_value_check Switch to enable checking whether the input is in the safe range of counter values
        //! @param invert Switch to invert PWMA and PWMB signals
        //! @param decouple_cc1 Switch to decouple CC1 from CC0 and steer them independently
        //! @param disable_a Switch to disable PWMA
        //! @param disable_b Switch to disable PWMB
        void setConfiguration(
            const uint32_t max_counter_value, const uint32_t dead_time, const uint32_t min_switch_time,
            UpdateType update_type = UpdateType::zero, const bool enable_pwm_check = true,
            const bool enable_shoot_through_check = true, const bool bypass_dead_time = false,
            const bool enable_value_check = true, const bool invert = false, const bool decouple_cc1 = false,
            const bool disable_a = false, const bool disable_b = false
        )
        {
            m_regs.ctrhSc.write(max_counter_value);
            m_regs.deadtimeSc.write(dead_time);
            m_regs.minSwitchTimeSc.write(min_switch_time);
            m_regs.cc0Sc.write(0);

            m_regs.config.updateType.set(update_type);
            m_regs.config.enablePwmCheck.set(enable_pwm_check);
            m_regs.config.enableStCheck.set(enable_shoot_through_check);
            m_regs.config.bypassDeadtime.set(bypass_dead_time);
            m_regs.config.enableValueCheck.set(enable_value_check);
            m_regs.config.invert.set(invert);
            m_regs.config.decoupleCc1.set(decouple_cc1);

            m_regs.config.disableA.set(disable_a);
            m_regs.config.disableB.set(disable_b);
        }

        void configureMinMaxModulation()
        {
            // Assumption is made t hat the configuration does not change at runtime, so configuration can be
            // internalized in memory rather than always read from register
            m_max_counter_value = m_regs.ctrhSc.read();

            const uint32_t dead_time = m_regs.deadtimeSc.read();   // dead time, in clock cycles
            const uint32_t min_switch_time
                = m_regs.minSwitchTimeSc.read();   // minimum ON or OFF switch time, in clock cycles

            // max counter value serves as period in clock cycles

            // min and max modulation indices, 0-1 (normalised to period)
            if (min_switch_time == 0)
            {
                if (m_regs.config.bypassDeadtime.get())
                {
                    m_max_modulation_index = 1.0;
                }
                else
                {
                    m_max_modulation_index = static_cast<float>(2 * m_max_counter_value - (dead_time + 1))
                                             / static_cast<float>(2.0 * m_max_counter_value);
                }
            }
            else
            {
                if (m_regs.config.bypassDeadtime.get())
                {
                    m_max_modulation_index = static_cast<float>(2 * m_max_counter_value - min_switch_time)
                                             / static_cast<float>(2.0 * m_max_counter_value);
                }
                else
                {
                    m_max_modulation_index
                        = static_cast<float>(2 * m_max_counter_value - (min_switch_time + dead_time + 1))
                          / static_cast<float>(2.0 * m_max_counter_value);
                }
            }
            m_min_modulation_index = 1.0 - m_max_modulation_index;

            // TMP: these values will be set by the configurator
            m_regs.minModIdxSc.write(m_min_modulation_index * 2 * m_max_counter_value);
            m_regs.maxModIdxSc.write(m_max_modulation_index * 2 * m_max_counter_value);
            // END OF TMP
        }

        //! Sets the desired modulation index.
        //!
        //! @param index Modulation index to be used, limited from 0 to 1 (at maximum)
        //! @param write_to_cc0 Flag to decide whether to write to CC0 (symmetrical case), or to CC1 (assymetry between
        //! CC0 and CC1)
        //! @return Boolean value with information whether the modulation index value set is identical to the one
        //! provided (true), false otherwise
        [[maybe_unused]] bool setModulationIndex(const float modulation_index, bool write_to_cc0 = true) noexcept
        {
            const float index     = forceLimit(modulation_index, m_min_modulation_index, m_max_modulation_index);
            const float threshold = getMaximumCounterValue() * index;

            if (isnanf(threshold))
            {
                // avoid setting NaN to register, return early
                return false;
            }
            if (write_to_cc0)
            {
                writeCC0(static_cast<uint32_t>(threshold));
            }
            else
            {
                writeCC1(static_cast<uint32_t>(threshold));
            }
            return (index == modulation_index);
        }

        //! Configures the PWM to stay high.
        void setHigh() noexcept
        {
            writeCC0(0);
        }

        //! Configures the PWM to stay low.
        void setLow() noexcept
        {
            writeCC0(getMaximumCounterValue());
        }

        //! Sets the update type.
        //!
        //! @param type Update type to be set, one of zero, period, zeroPeriod, immediate
        void setUpdateType(const UpdateType type) noexcept
        {
            m_regs.config.updateType.set(type);
        }

        //! Sets the additional extended dead time.
        //!
        //! @param additional_dead_time Additional dead time, in clock ticks
        void setExtendedDeadTime(const uint32_t additional_dead_time) noexcept
        {
            m_regs.extendedDeadtimeSc.write(additional_dead_time);
        }

        //! Sets the enabled status for PWMA.
        //!
        //! @param setting Flag for setting the PWMA enabled status: enabled if true, disabled otherwise
        void setEnabledA(const bool setting) noexcept
        {
            m_regs.config.disableA.set(!setting);
        }

        //! Sets the enabled status for PWMB.
        //!
        //! @param setting Flag for setting the PWMB enabled status: enabled if true, disabled otherwise
        void setEnabledB(const bool setting) noexcept
        {
            m_regs.config.disableB.set(!setting);
        }

        //! Enables the PWM counters.
        //!
        //! @param setting Setting for the counters,
        void setEnabled(const bool setting) noexcept
        {
            m_regs.ctrl.enable.set(setting);
        }

        //! Sets the inverted status of PWMA and PWMB.
        //!
        //! @param setting Flag for setting the PWM inverted status: inverted if true, not inverted otherwise
        void setInverted(const bool setting) noexcept
        {
            m_regs.config.invert.set(setting);
        }

        //! Sets decoupling CC1 from CC0 to be used independently, e.g. in an inverter.
        //!
        //! @param setting Flag for setting the CC1 independently from CC0 (true), false if coupling is desired
        //! (default).
        void setDecoupleCC1(const bool setting) noexcept
        {
            symmetrical = !setting;
            m_regs.config.decoupleCc1.set(setting);
        }

        //! Reads the maximum counter value.
        //!
        //! @return Maximum counter value in clock ticks
        uint32_t getMaximumCounterValue() const noexcept
        {
            return m_max_counter_value;
        }

        //! Reads the modulation index value (CC0).
        //!
        //! @return Modulation index set to CC0
        uint32_t getModulationIndex() const noexcept
        {
            return m_regs.cc0Sc.read();
        }

        //! Reads the modulation index value (CC1).
        //!
        //! @return Modulation index set to CC1
        uint32_t getModulationIndexCC1() const noexcept
        {
            return m_regs.carrierBits.read();
        }

        //! Returns the constant size of PWM
        //!
        //! @return Maximum size of the PWM registers
        static auto constexpr size() noexcept
        {
            return ipCores::Top::PwmArrayItem::size;
        }

        //   private:
        ipCores::Top::PwmArrayItem::Pwm m_regs;

        //! Maximum counter value to which the PWM counter is counting to, a configuration parameter.
        uint32_t m_max_counter_value{0};
        //! Minimum modulation index that can be set for this particular PWM.
        float m_min_modulation_index{-1.0};
        //! Maximum modulation index that can be set for this particular PWM.
        float m_max_modulation_index{1.0};

        //! Flag to signal whether CC0 and CC1 are equal
        bool symmetrical{true};

        //! Writes to the PWM trigger register CC0, allows to set the trigger for PWM to go high.
        //!
        //! @param threshold Threshold, in clock ticks, for PWM to go high
        void writeCC0(uint32_t threshold) noexcept
        {
            m_regs.cc0Sc.write(threshold);
        }

        //! Writes to the PWM trigger register CC1, allows to set the trigger for PWM to go high.
        //!
        //! @param threshold Threshold, in clock ticks, for PWM to go high
        void writeCC1(uint32_t threshold) noexcept
        {
            m_regs.cc1Sc.write(threshold);
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