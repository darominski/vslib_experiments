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
    template<uint32_t first_pwm_id>
    class TwoLevelAFE : public Component
    {
      public:
        TwoLevelAFE(std::string_view name, Component& parent)
            : Component("TwoLevelActiveFrontEnd", name, parent),
              leg_1("leg_1", *this),
              leg_2("leg_2", *this),
              leg_3("leg_3", *this)
        {
        }

        // ************************************************************
        // Start and stop methods

        void start() noexcept
        {
            leg_1.start();
            leg_2.start();
            leg_3.start();
        }

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
        HalfBridge<first_pwm_id>     leg_1;   //!< Leg 1 of the Ftwo-level AFE
        HalfBridge<first_pwm_id + 1> leg_2;   //!< Leg 2 of the two-level AFE
        HalfBridge<first_pwm_id + 2> leg_3;   //!< Leg 3 of the FUll Bridge
    };
}