//! @file
//! @brief Defines the Component implementing the abc to alpha-beta transform from three phase to two-phase.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"

namespace vslib
{
    class AlphaBetaToAbcTransform : public Component
    {
      public:
        //! Creates the alpha-beta-zero to a-b-c transformation Component
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        AlphaBetaToAbcTransform(std::string_view name, IComponent& parent)
            : Component("AlphaBetaToAbcTransform", name, parent)
        {
        }

        //! Performs alpha-beta-zero to a-b-c transform on the provided inputs
        //!
        //! @param f_alpha alpha-component of alpha-beta-zero-frame component
        //! @param f_beta beta-component of alpha-beta-zero-frame component
        //! @param f_0 zero-component of alpha-beta-zero-frame component
        //! @return Tuple of alpha, beta, zero values
        [[nodiscard]] std::tuple<double, double, double>
        transform(const double f_alpha, const double f_beta, const double f_0) noexcept;
    };
}   // namespace vslib