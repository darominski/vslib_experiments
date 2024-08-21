//! @file
//! @brief Defines the Component implementing the dq0 to abc transform.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"

namespace vslib
{
    class Dq0ToAbcTransform : public Component
    {
      public:
        //! Creates the Dq0ToAbcTransform Component.
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        Dq0ToAbcTransform(std::string_view name, IComponent& parent)
            : Component("Dq0ToAbcTransform", name, parent)
        {
        }

        //! Performs d-q-0 to a-b-c transform from a dq0 rotating reference frame to three-phase (abc). Angular
        //! position of the rotating frame is given by input wt. By default, the a-phase to q-axis alignment is
        //! assumed but this can be modified with the manual offset parameter.
        //!
        //! @param d d value
        //! @param q q value
        //! @param zero zero value
        //! @param wt angle (in radians) between q and a
        //! @param offset Offset angle alignment (in radians): 0 is q alignment, pi/2 for d and a alignment
        //! @return Tuple of a, b, c values
        [[nodiscard]] std::tuple<double, double, double>
        transform(const double d, const double q, const double zero, const double wt, const double offset = 0) noexcept;
    };
}   // namespace vslib