//! @file
//! @brief Defines the Component class for a look-up table holding trigonometry functions.
//! @author Dominik Arominski

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>

#include "component.h"
#include "periodicLookupTable.h"

namespace vslib
{
    enum class TrigonometryFunctions
    {
        sin,
        cos
    };

    class TrigonometryLookupTable : public Component
    {
      public:
        TrigonometryLookupTable(
            std::string_view name, Component* parent, TrigonometryFunctions choice, size_t number_points
        )
            : Component("TrigonometryLookupTable", name, parent),
              m_function("data", this)
        {
            std::vector<std::pair<double, double>> data(number_points);
            for (size_t index = 0; index < number_points; index++)
            {
                const double x = (2.0 * M_PI * index) / number_points;   // 0 - 2pi
                double       y;
                if (choice == TrigonometryFunctions::sin)
                {
                    y = sin(x);
                }
                else if (choice == TrigonometryFunctions::cos)
                {
                    y = cos(x);
                }
                data[index] = std::make_pair(x, y);
            }
            m_function.setData(std::move(data), true);
        }

        //! Operator overload providing more intuitive interaction with the underlying table
        double operator()(double input_x)
        {
            return m_function.interpolate(input_x);
        }

      private:
        PeriodicLookupTable<double, double> m_function;
    };
}