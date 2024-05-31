//! @file
//! @brief Defines the Component class for a look-up table holding sine function.
//! @author Dominik Arominski

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>

#include "component.h"
#include "periodicLookupTable.h"

namespace vslib
{

    class SinLookupTable : public Component
    {
      public:
        SinLookupTable(std::string_view name, Component* parent, size_t number_points)
            : Component("SinLookupTable", name, parent),
              m_function("data", this)
        {
            assert(number_points >= 2);
            std::vector<std::pair<double, double>> data(number_points + 1);
            for (size_t index = 0; index <= number_points; index++)
            {
                const double x = (2.0 * M_PI * index) / number_points;   // 0 - 2pi
                double       y = sin(x);
                data[index]    = std::make_pair(x, y);
            }
            m_function.setData(std::move(data), true);
        }

        //! Provides main access interaction with the underlying table consistent with LookupTable components
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        double interpolate(double input_x)
        {
            return m_function.interpolate(input_x);
        }

        //! Provides intuitive interaction with the underlying table
        //!
        //! @param input_x Value to be looked up in the table
        //! @return Interpolated function value closest to the input_x
        double operator()(double input_x)
        {
            return m_function.interpolate(input_x);
        }

      private:
        PeriodicLookupTable<double, double> m_function;
    };
}