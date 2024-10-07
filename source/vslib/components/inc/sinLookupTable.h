//! @file
//! @brief Defines the Component class for a look-up table holding sine function.
//! @author Dominik Arominski

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <numbers>
#include <string>

#include "component.h"
#include "functionGenerator.h"
#include "periodicLookupTable.h"

namespace vslib
{
    class SinLookupTable : public Component
    {
      public:
        //! Constructor for the SinLookupTable Component.
        //!
        //! @param name Name of this Component
        //! @param parent Parent of this Component
        //! @param number_points Length of the lookup table vector
        SinLookupTable(std::string_view name, Component& parent, size_t number_points)
            : Component("SinLookupTable", name, parent),
              m_function(
                  "data", *this,
                  fgc4::utils::generateFunction<double, double>(sin, 0.0, 2.0 * std::numbers::pi, number_points), true
              )
        {
            assert(number_points >= 2);
        }

        //! Provides the main access interaction with the underlying table consistent with LookupTable components.
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodiscard]] auto interpolate(const double input_x)
        {
            return m_function.interpolate(input_x);
        }

        //! Provides intuitive interaction with the underlying table.
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodiscard]] auto operator()(const double input_x)
        {
            return m_function.interpolate(input_x);
        }

      private:
        //!< Component providing the sine function storage and interpolation functionalities
        PeriodicLookupTable<double, double> m_function;
    };
}   // namespace vslib
