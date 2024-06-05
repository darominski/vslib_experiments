//! @file
//! @brief Defines the Component implementing the Park transform from three phase to two-phase.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "cosLookupTable.h"
#include "sinLookupTable.h"

namespace vslib
{
    namespace
    {
        constexpr double SQRT_3 = std::sqrt(3.0);
    }

    class ParkTransform : public Component
    {
      public:
        ParkTransform(std::string_view name, Component* parent, uint64_t number_points = 1000)
            : Component("ParkTransform", name, parent),
              m_sin("sin", this, number_points),
              m_cos("cos", this, number_points)
        {
        }

        std::pair<double, double> transform(double a, double b, double c, double theta) noexcept;

      private:
        SinLookupTable m_sin;
        CosLookupTable m_cos;
    };
}   // namespace vslib