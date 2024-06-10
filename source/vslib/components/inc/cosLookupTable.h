//! @file
//! @brief Defines the Component class for a look-up table holding cosine function.
//! @author Dominik Arominski

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>

#include "component.h"
#include "functionGenerator.h"
#include "periodicLookupTable.h"

namespace vslib
{
    class CosLookupTable : public Component
    {
      public:
        CosLookupTable(std::string_view name, Component* parent, size_t number_points)
            : Component("CosLookupTable", name, parent),
              m_function(
                  "data", this, fgc4::utils::generateFunction<double, double>(cos, 0.0, 2.0 * M_PI, number_points), true
              )
        {
            assert(number_points >= 2);
        }

        //! Provides intuitive interaction with the underlying table
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodiscard]] auto interpolate(double input_x)
        {
            return m_function.interpolate(input_x);
        }

        //! Provides intuitive interaction with the underlying table
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodiscard]] auto operator()(double input_x)
        {
            return m_function.interpolate(input_x);
        }

      private:
        PeriodicLookupTable<double, double> m_function;
    };
}   // namespace vslib