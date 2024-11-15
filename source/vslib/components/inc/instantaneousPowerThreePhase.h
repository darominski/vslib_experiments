//! @file
//! @brief Defines the Component implementing the calculation of the instantaneous power of a three-phase system.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    class InstantaneousPowerThreePhase : public Component
    {
      public:
        //! Creates the abc to alpha-beta transformation Component
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        InstantaneousPowerThreePhase(std::string_view name, Component& parent)
            : Component("InstantaneousPowerThreePhase", name, parent),
              p_gain(*this, "p_gain"),
              q_gain(*this, "q_gain")
        {
        }

        //! Performs the calculation of the active and reactive power of the a-b-c input
        //!
        //! @param v_a a-phase voltage value
        //! @param v_b b-phase voltage value
        //! @param v_c c-phase voltage value
        //! @param i_a a-phase current value
        //! @param i_b b-phase current value
        //! @param i_c c-phase current value
        //! @return Tuple of P, and Q: active and reactive power
        [[nodiscard]] std::tuple<double, double> calculate(
            const double v_a, const double v_b, const double v_c, const double i_a, const double i_b, const double i_c
        ) const noexcept;

        Parameter<double> p_gain;   //!< Gain of the active power component
        Parameter<double> q_gain;   //!< Gain of the reactive power component
    };
}   // namespace vslib