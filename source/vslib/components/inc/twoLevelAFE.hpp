//! @file
//! @brief Component implementing a two-level active front-end functionality based on three PWMs.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.hpp"
#include "halfBridge.hpp"
#include "parameter.hpp"

namespace vslib
{
    class TwoLevelAFE : public Component
    {
      public:
        //! Constructor for the TwoLevelAFE Component.
        //!
        //! @param name Name of this Component
        //! @param parent Parent of this Component
        //! @param first_pwm_id ID of the first FPGA-defined PWM associated to this Component
        //! @param max_counter_value Maximal value of the PWM counter (half-period length)
        TwoLevelAFE(
            std::string_view name, Component& parent, const uint32_t first_pwm_id, const uint32_t max_counter_value
        )
            : Component("TwoLevelActiveFrontEnd", name, parent),
              leg_1("leg_1", *this, first_pwm_id, max_counter_value),
              leg_2("leg_2", *this, first_pwm_id + 1, max_counter_value),
              leg_3("leg_3", *this, first_pwm_id + 2, max_counter_value)
        {
        }

        // ************************************************************
        // Start and stop methods

        //! Starts all PWMs.
        void start() noexcept
        {
            leg_1.start();
            leg_2.start();
            leg_3.start();
        }

        //! Stops all PWMs
        void stop() noexcept
        {
            leg_1.stop();
            leg_2.stop();
            leg_3.stop();
        }

        // ************************************************************
        // Interaction methods with legs

        //! Sets the modulation index of the leg 1 of the two-level AFE.
        //!
        //! @param modulation_index Modulation index, from -1.0 to 1.0, will be set to leg 1
        void setModulationIndexLeg1(const float modulation_index) noexcept
        {
            leg_1.setModulationIndex(modulation_index);
        }

        //! Sets the modulation index of the leg 2 of the two-level AFE.
        //!
        //! @param modulation_index Modulation index, from -1.0 to 1.0, will be set to leg 2
        void setModulationIndexLeg2(const float modulation_index) noexcept
        {
            leg_2.setModulationIndex(modulation_index);
        }

        //! Sets the modulation index of the leg 3 of the two-level AFE.
        //!
        //! @param modulation_index Modulation index, from -1.0 to 1.0, will be set to leg 3
        void setModulationIndexLeg3(const float modulation_index) noexcept
        {
            leg_3.setModulationIndex(modulation_index);
        }

        //! Sets the modulation index of all the legs of the two-level AFE.
        //!
        //! @param modulation_index_1 Modulation index for leg 1, from -1.0 to 1.0
        //! @param modulation_index_2 Modulation index for leg 2, from -1.0 to 1.0
        //! @param modulation_index_3 Modulation index for leg 3, from -1.0 to 1.0
        void setModulationIndexLeg1(
            const float modulation_index_1, const float modulation_index_2, const float modulation_index_3
        ) noexcept
        {
            leg_1.setModulationIndex(modulation_index_1);
            leg_2.setModulationIndex(modulation_index_2);
            leg_3.setModulationIndex(modulation_index_3);
        }

      private:
        HalfBridge leg_1;   //!< Leg 1 of the Ftwo-level AFE
        HalfBridge leg_2;   //!< Leg 2 of the two-level AFE
        HalfBridge leg_3;   //!< Leg 3 of the FUll Bridge
    };
}