//! @file
//! @brief Defines the Component implementing the dq0 to alpha-beta-zero transform.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "cosLookupTable.h"
#include "sinLookupTable.h"

namespace vslib
{
    class Dq0ToAlphaBetaTransform : public Component
    {
      public:
        //! Creates the Dq0ToAlphaBetaTransform Component.
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        Dq0ToAlphaBetaTransform(std::string_view name, IComponent& parent, const uint64_t number_points = 10'000)
            : Component("Dq0ToAlphaBetaTransform", name, parent),
              m_sin("sin", *this, number_points),
              m_cos("cos", *this, number_points)
        {
        }

        //! Performs d-q-0 to a-b-c transform from a dq0 rotating reference frame to three-phase (abc). Angular
        //! position of the rotating frame is given by input wt. By default, the a-phase to q-axis alignment is
        //! assumed but this can be modified with the manual offset parameter.
        //!
        //! @param d d value
        //! @param q q value
        //! @param zero zero value
        //! @param theta angle (in radians) between q and a
        //! @param a_alignment Whether the frame alignment at t=0 is aligned with A-axis (true) or 90 degrees behind
        //! A-axis (false)
        //! @return Tuple of alpha, beta, zero values
        [[nodiscard]] std::tuple<double, double, double> transform(
            const double d, const double q, const double zero, const double theta, const bool a_alignment = true
        ) noexcept;

      private:
        SinLookupTable m_sin;   //!< Lookup table holding sine function
        CosLookupTable m_cos;   //!< Lookup table holding cosine function
    };
}   // namespace vslib