//! @file
//! @brief Defines the Component implementing the abc to dq0 transform from three phase to two-phase.
//! @author Dominik Arominski

#pragma once

#include <string>
#include <tuple>

#include "abcToAlphaBetaTransform.hpp"
#include "alphaBetaToDq0Transform.hpp"
#include "component.hpp"

namespace vslib
{
    class AbcToDq0Transform : public Component
    {
      public:
        //! Creates the AbcToDq0Transform Component.
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        //! @param number_points Number of points for the sine and cosine lookup tables
        AbcToDq0Transform(std::string_view name, Component& parent, const uint64_t number_points = 10'000)
            : Component("AbcToDq0Transform", name, parent),
              m_abc_to_alphabeta("abc_to_alphabeta", *this),
              m_alphabeta_to_dq0("alphabeta_to_dq0", *this, number_points)
        {
        }

        //! Performs a-b-c to d-q-0 transform from a three-phase (abc) to a dq0 rotating reference frame. Angular
        //! position of the rotating frame is given by input wt. By default, the a-phase to q-axis alignment is
        //! assumed but this can be modified with the manual offset parameter.
        //!
        //! @param f_a a-phase value
        //! @param f_b b-phase value
        //! @param f_c c-phase value
        //! @param wt angle (in radians) between q and a
        //! @param offset Offset angle alignment (in radians): 0 is q alignment, pi/2 for d and a alignment
        //! @return Tuple of d, q, 0 values
        [[nodiscard]] std::tuple<double, double, double> transform(
            const double f_a, const double f_b, const double f_c, const double wt, const double offset = 0
        ) noexcept;

      private:
        AbcToAlphaBetaTransform m_abc_to_alphabeta;
        AlphaBetaToDq0Transform m_alphabeta_to_dq0;
    };
}   // namespace vslib
