//! @file
//! @brief Defines the Component implementing the Clarke transform from three phase to two-phase.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "cosLookupTable.h"
#include "sinLookupTable.h"

namespace vslib
{
    class ClarkeTransform : public Component
    {
      public:
        //! Creates the Clarke transformation Component
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        ClarkeTransform(std::string_view name, IComponent& parent)
            : Component("ClarkeTransform", name, parent)
        {
        }

        //! Performs Clarke transform on the provided inputs
        //!
        //! @param f_a a-phase value of abs-frame component
        //! @param f_b b-phase value of abs-frame component
        //! @param f_c c-phase value of abs-frame component
        //! @return Tuple of alpha, beta, zero orthogonal values in the stationary alpha-beta reference frame
        [[nodiscard]] std::tuple<double, double, double>
        transform(const double f_a, const double f_b, const double f_c) noexcept;
    };
}   // namespace vslib