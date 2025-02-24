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
        FullBridge(std::string_view name, Component& parent, uint8_t* base_address)
            : Component("FullBridge", name, parent),
              leg_1("leg_1", *this, base_address),
              leg_2("leg_2", *this, base_address + leg_1.size())
        {
        }

        // ************************************************************
        // Start and stop methods

        void start() noexcept
        {
            leg_1.start();
            leg_2.start();
        }

        void stop() noexcept
        {
            leg_1.stop();
            leg_2.stop();
        }

        void reset() noexcept
        {
            leg_1.reset();
            leg_2.reset();
        }

        // ************************************************************
        // Interaction methods with legs

        //! Sets the modulation index of a two-level unipolar full bridge.
        //!
        //! @param modulation_index Modulation index, from -1.0 to 1.0, will be set to leg 2
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
        //! @param modulation_index Modulation index, from -1.0 to 1.0, will be set to leg 1
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
        //! @param modulation_index Modulation index, from -1.0 to 1.0, will be set to both leg 1 and 2
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

            leg_1.setModulationIndex(modulation_index);
            leg_2.setModulationIndex(-modulation_index);
        }

        //! Sets the duty cycle of positive unipolar two-level full bridge.
        //!
        //! @param duty_cycle Duty cycle of the full bridge, will be set to leg 2
        void setDutyCyclePositive(const float duty_cycle) noexcept
        {
            leg_1.setDutyCycle(1.0);
            leg_2.setDutyCycle(duty_cycle);
        }


        //! Sets the duty cycle of negative unipolar two-level full bridge.
        //!
        //! @param duty_cycle Duty cycle of the full bridge, will be set to leg 1
        void setDutyCycleNegative(const float duty_cycle) noexcept
        {
            leg_1.setDutyCycle(duty_cycle);
            leg_2.setDutyCycle(1.0);
        }

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            return {};
        }

      private:
        HalfBridge leg_1;   //!< Leg 1 of the Full Bridge
        HalfBridge leg_2;   //!< Leg 2 of the FUll Bridge

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
            leg_2.invertA(setting);
            leg_2.invertB(!setting);
        }
    };
}