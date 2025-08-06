//! @file
//! @brief Component implementing a full-bridge functionality based on PWMs.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.hpp"
#include "halfBridge.hpp"
#include "parameter.hpp"

namespace vslib
{
    class FullBridge : public Component
    {
      public:
        //! Full Bridge Component constructor.
        //!
        //! @param name Name of this FullBridge instance
        //! @param parent Parent of this FullBridge instance
        //! @param pwm_id ID of the first FPGA's PWM that this HalfBridge interacts with
        //! @param max_counter_value Specifies the maximal counter value of the owned PWMs, impacting their
        //! frequency. This argument will be removed once the Configurator is available.
        FullBridge(
            std::string_view name, Component& parent, const uint32_t first_pwm_id, const uint32_t max_counter_value
        )
            : Component("FullBridge", name, parent),
              leg_1("leg_1", *this, first_pwm_id, max_counter_value),
              leg_2("leg_2", *this, first_pwm_id + 1, max_counter_value)
        {
        }

        // ************************************************************
        // Start and stop methods

        //! Starts the owned PWM counters.
        void start() noexcept
        {
            leg_1.start();
            leg_2.start();
        }

        //! Stops the owned PWM counters.
        void stop() noexcept
        {
            leg_1.stop();
            leg_2.stop();
        }

        // ************************************************************
        // Interaction methods with legs

        //! Sets the modulation index of a two-level unipolar full bridge.
        //!
        //! @param modulation_index Modulation index, from 0.0 to 1.0, will be set to leg 2
        void setModulationIndexPositive(const float modulation_index) noexcept
        {
            if (m_bipolar)
            {
                switchBipolar(false);
            }

            leg_1.setHigh();
            leg_2.setModulationIndex(modulation_index);
        }

        //! Sets the modulation index of a two-level unipolar full bridge.
        //!
        //! @param modulation_index Modulation index, from 0.0 to 1.0, will be set to leg 1
        void setModulationIndexNegative(const float modulation_index) noexcept
        {
            if (m_bipolar)
            {
                switchBipolar(false);
            }
            leg_1.setModulationIndex(modulation_index);
            leg_2.setHigh();
        }

        //! Sets the modulation index of a two-level bipolar full bridge.
        //!
        //! @param modulation_index Modulation index, from 0.0 to 1.0, will be set to both leg 1 and 2
        void setModulationIndex2L1Fsw(const float modulation_index) noexcept
        {
            leg_1.setModulationIndex(modulation_index);

            // leg 2 should be configured to be an inverse of leg 1, but since we can switch
            // from different modes, a change of setting should occur at some point
            if (!m_bipolar)
            {
                switchBipolar(true);
            }
            leg_2.setModulationIndex(modulation_index);
        }

        //! Sets the modulation index of a three-level full bridge.
        //!
        //! @param modulation_index Modulation index, from -1.0 to 1.0, will be set to leg 1, and its negative
        //! counterpart to leg 2
        void setModulationIndex3L2Fsw(const float modulation_index) noexcept
        {
            if (m_bipolar)
            {
                switchBipolar(false);
            }

            // recalculate modulation index to fit in 0 to 1:
            const float modulatin_index_leg_1 = 0.5 * (modulation_index + 1);
            const float modulatin_index_leg_2 = 0.5 * (-modulation_index + 1);

            leg_1.setModulationIndex(modulatin_index_leg_1);
            leg_2.setModulationIndex(modulatin_index_leg_2);
        }

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            return {};
        }

      private:
        HalfBridge leg_1;   //!< Leg 1 of the Full Bridge
        HalfBridge leg_2;   //!< Leg 2 of the Full Bridge

        //! Flag informing whether the full bridge is running in bipolar mode
        bool m_bipolar{false};
        // ************************************************************

        //! Switches the bipolar mode according to the setting. Inverts the PWMA and PWMB signals.
        //!
        //! @param setting If true, the bipolar mode is enabled and leg 2's PWMA and PWMB are inverted, false otherwise
        //! (default)
        void switchBipolar(const bool setting) noexcept
        {
            m_bipolar = setting;
            leg_2.setInverted(setting);
        }
    };
}