//! @file
//! @brief Defines the Component implementing the abc to dq0 transform from three phase to two-phase.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "cosLookupTable.h"
#include "sinLookupTable.h"

namespace vslib
{
    class AbcToDq0Transform : public Component
    {
      public:
        //! Creates the AbcToDq0Transform Component with the LookupTables of the requested size.
        //!
        //! @param name Name of the Component
        //! @param parent Parent of this Component
        //! @param number_points Number of points for the sine and cosine lookup tables
        AbcToDq0Transform(std::string_view name, IComponent& parent, const uint64_t number_points = 1000)
            : Component("AbcToDq0Transform", name, parent),
              m_sin("sin", *this, number_points),
              m_cos("cos", *this, number_points)
        {
        }

        //! Performs a-b-c to d-q-0 transform from a three-phase (abc) to a dq0 rotating reference frame. Angular
        //! position of the rotating frame is given by input theta. By default, the a-phase to q-axis alignment is
        //! assumed but this can be modified with the manual offset parameter.
        //!
        //! @param a a-phase value
        //! @param b b-phase value
        //! @param c c-phase value
        //! @param theta theta angle (in radians) between q and a
        //! @param offset Offset angle alignment (in radians): 0 is q alignment, pi/2 for d and a alignment
        //! @return Tuple of d, q, 0 values
        [[nodiscard]] std::tuple<double, double, double>
        transform(const double a, const double b, const double c, const double theta, const double offset = 0) noexcept;

      private:
        SinLookupTable m_sin;   //!< Lookup table holding sine function
        CosLookupTable m_cos;   //!< Lookup table holding cosine function
    };
}   // namespace vslib