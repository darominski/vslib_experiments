//! @file
//! @brief Defines the Component implementing the abc to alpha-beta transform from three phase to two-phase.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>
#include <tuple>

#include "component.hpp"

namespace vslib
{
    class AbcToAlphaBetaTransform : public Component
    {
      public:
        //! Creates the abc to alpha-beta transformation Component
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        AbcToAlphaBetaTransform(std::string_view name, Component& parent)
            : Component("AbcToAlphaBetaTransform", name, parent)
        {
        }

        //! Performs a-b-c to alpha-beta transform on the provided inputs
        //!
        //! @param f_a a-phase value of abc-frame component
        //! @param f_b b-phase value of abc-frame component
        //! @param f_c c-phase value of abc-frame component
        //! @return Tuple of alpha, beta, zero orthogonal values in the stationary alpha-beta reference frame
        [[nodiscard]] std::tuple<double, double, double>
        transform(const double f_a, const double f_b, const double f_c) const noexcept;
    };
}   // namespace vslib