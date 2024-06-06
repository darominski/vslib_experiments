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
        ClarkeTransform(std::string_view name, Component* parent, uint64_t number_points = 1000)
            : Component("ClarkeTransform", name, parent)
        {
        }

        //! Performs Clarke transform on the provided inputs
        //!
        //! @param f_a a-phase value of abs-frame component
        //! @param f_b b-phase value of abs-frame component
        //! @param f_c c-phase value of abs-frame component
        //! @return Pair of alpha, beta orthogonal values in the stationary reference frame
        std::pair<double, double> transform(double f_a, double f_b, double f_c) noexcept;
    };
}   // namespace vslib