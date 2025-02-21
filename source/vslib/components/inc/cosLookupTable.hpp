//! @file
//! @brief Defines the Component class for a look-up table holding cosine function.
//! @author Dominik Arominski

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <numbers>
#include <string>

#include "component.hpp"
#include "functionGenerator.hpp"
#include "periodicLookupTable.hpp"

namespace vslib
{
    class CosLookupTable : public Component
    {
      public:
        //! Constructor for the CosLookupTable Component.
        //!
        //! @param name Name of this Component
        //! @param parent Parent of this Component
        //! @param number_points Length of the lookup table vector
        CosLookupTable(std::string_view name, Component& parent, const size_t number_points)
            : Component("CosLookupTable", name, parent),
              m_function(
                  "data", *this,
                  fgc4::utils::generateFunction<double, double>(cos, 0.0, 2.0 * std::numbers::pi, number_points), true
              )
        {
            assert(number_points >= 2);
        }

        //! Provides intuitive interaction with the underlying table
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodiscard]] auto interpolate(const double input_x)
        {
            return m_function.interpolate(input_x);
        }

        //! Provides intuitive interaction with the underlying table
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodiscard]] auto operator()(const double input_x)
        {
            return m_function.interpolate(input_x);
        }

      private:
        //!< Table holding the cosine function and providing interpolation functionality
        PeriodicLookupTable<double, double> m_function;
    };
}   // namespace vslib