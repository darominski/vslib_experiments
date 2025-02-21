//! @file
//! @brief Defines the Component implementing the alpha-beta-zero to dq0 transformation.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.hpp"
#include "cosLookupTable.hpp"
#include "sinLookupTable.hpp"

namespace vslib
{
    class AlphaBetaToDq0Transform : public Component
    {
      public:
        //! Creates the alpha-beta to dq0 transformation Component.
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        //! @param number_points Number of points for the sine and cosine lookup tables
        AlphaBetaToDq0Transform(std::string_view name, Component& parent, const uint64_t number_points = 1000)
            : Component("AlphaBetaToDq0Transform", name, parent),
              m_sin("sin", *this, number_points),
              m_cos("cos", *this, number_points)
        {
        }

        //! Performs alpha-beta-zero to dq0 transform.
        //!
        //! @param f_alpha alpha-component of alpha-beta-zero-frame component
        //! @param f_beta beta-component of alpha-beta-zero-frame component
        //! @param f_0 zero-component of alpha-beta-zero-frame component
        //! @param wt angle (in radians) between q and alpha
        //! @param a_alignment Whether the frame alignment at t=0 is aligned with A-axis (true) or 90 degrees behind
        //! A-axis (false)
        //! @return Tuple of d, q, 0 values
        [[nodiscard]] std::tuple<double, double, double> transform(
            const double f_alpha, const double f_beta, const double f_0, const double wt, const bool a_alignment = true
        ) noexcept;

      private:
        SinLookupTable m_sin;   //!< Lookup table holding sine function
        CosLookupTable m_cos;   //!< Lookup table holding cosine function
    };
}   // namespace vslib
