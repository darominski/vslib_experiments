//! @file
//! @brief Defines the Component implementing the alpha-beta-zero to dq0 (Clarke components to Park components)
//! transformation.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "cosLookupTable.h"
#include "sinLookupTable.h"

namespace vslib
{
    class AlphaBetaZeroToDq0Transform : public Component
    {
      public:
        //! Creates the ClarkePark transformation Component
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        AlphaBetaZeroToDq0Transform(std::string_view name, Component* parent, uint64_t number_points = 1000)
            : Component("AlphaBetaZeroToDq0Transform", name, parent),
              m_sin("sin", this, number_points),
              m_cos("cos", this, number_points)
        {
        }

        //! Performs Alpha-Beta-Zero to dq0 transform.
        //!
        //! @param f_alpha alpha-component of alpha-beta-zero-frame component
        //! @param f_beta beta-component of alpha-beta-zero-frame component
        //! @param f_0 zero-component of alpha-beta-zero-frame component
        //! @param theta theta angle (in radians) between q and alpha
        //! @param a_alignment Whether the frame alignment at t=0 is aligned with A-axis (true) or 90 degrees behind
        //! A-axis (false)
        //! @return Tuple of d, q, 0 values
        [[nodiscard]] std::tuple<double, double, double>
        transform(double f_alpha, double f_beta, double f_0, double theta, bool a_alignment = true) noexcept;

      private:
        SinLookupTable m_sin;   //!< Lookup table holding sine function
        CosLookupTable m_cos;   //!< Lookup table holding cosine function
    };
}   // namespace vslib