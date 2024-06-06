//! @file
//! @brief Defines the Component implementing the Park transform from three phase to two-phase.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "cosLookupTable.h"
#include "sinLookupTable.h"

namespace vslib
{
    class ParkTransform : public Component
    {
      public:
        ParkTransform(std::string_view name, Component* parent, uint64_t number_points = 1000)
            : Component("ParkTransform", name, parent),
              m_sin("sin", this, number_points),
              m_cos("cos", this, number_points)
        {
        }

        //! Performs Park transform on the provided inputs assuming a-phase to q-axis alignment
        //!
        //! @param a a-phase value
        //! @param b b-phase value
        //! @param c c-phase value
        //! @param theta theta angle between d and a
        //! @return Pair of d, q values
        std::pair<double, double> transform(double a, double b, double c, double theta) noexcept;

      private:
        SinLookupTable m_sin;
        CosLookupTable m_cos;
    };
}   // namespace vslib