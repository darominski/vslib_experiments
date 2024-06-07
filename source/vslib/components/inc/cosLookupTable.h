//! @file
//! @brief Defines the Component class for a look-up table holding cosine function.
//! @author Dominik Arominski

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>

#include "component.h"
#include "periodicLookupTable.h"

namespace vslib
{
    namespace
    {
        std::vector<std::pair<double, double>> cosine_function(size_t number_points)
        {
            std::vector<std::pair<double, double>> data(number_points + 1);
            for (size_t index = 0; index <= number_points; index++)
            {
                const double x = (2.0 * M_PI * index) / number_points;   // 0 - 2pi
                const double y = cos(x);
                data[index]    = std::make_pair(x, y);
            }
            return data;
        }
    }

    class CosLookupTable : public Component
    {
      public:
        CosLookupTable(std::string_view name, Component* parent, size_t number_points)
            : Component("CosLookupTable", name, parent),
              m_function("data", this, cosine_function(number_points))
        {
            assert(number_points >= 2);
        }

        //! Provides intuitive interaction with the underlying table
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodicard]] auto interpolate(double input_x)
        {
            return m_function.interpolate(input_x);
        }

        //! Provides intuitive interaction with the underlying table
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        [[nodicard]] auto operator()(double input_x)
        {
            return m_function.interpolate(input_x);
        }

      private:
        PeriodicLookupTable<double, double> m_function;
    };
}   // namespace vslib