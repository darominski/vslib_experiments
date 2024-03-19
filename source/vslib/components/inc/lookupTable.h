//! @file
//! @brief Defines the Component class for look-up table.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "parameter.h"
#include "typeTraits.h"

namespace vslib
{
    template<fgc4::utils::NumericScalar T>
    class LookupTable : public Component
    {
      public:
        //! Defines constructor for LookupTable component
        //!
        //! @param name Name of the LookupTable component object
        //! @param parent Pointer to the parent of this table
        //! @param values Vector with x-y pairs of the function to be stored
        LookupTable(std::string_view name, Component* parent, std::vector<std::pair<T, T>>&& values)
            : Component("LookupTable", name, parent),
              m_values{std::move(values)}
        {
        }

        //! For provided x-axis input provides an interpolated y-axis value from the stored values
        //!
        //! @param input_x X-axis input value to interpolate
        //! @return Y-axis value result of the interpolation
        T interpolate(T input_x) noexcept
        {
            size_t start_loop_index = 0;
            size_t end_loop_index   = m_values.size();
            if (input_x >= m_previous_input)
            {
                start_loop_index = m_previous_index;
            }
            else
            {
                end_loop_index = m_previous_index;
            }

            T      min_value{std::numeric_limits<T>::max()};
            size_t min_index{m_values.size()};
            for (size_t index = start_loop_index; index < end_loop_index; index++)
            {
                auto const diff = abs(m_values[index].first - input_x);
                if (min_value > diff)
                {
                    min_value = diff;
                    min_index = index;
                }
                else   // assuming monotonic distribution of x axis values
                {
                    break;
                }
            }

            if (min_index > (m_values.size() - 2))
            {
                fgc4::utils::Warning(fmt::format(
                    "Interpolation error: provided input value: {} outside of provided look-up table bounds of: [{}, "
                    "{}].\n",
                    input_x, m_values[0].first, m_values[m_values.size() - 1].first
                ));
                return T{};
            }
            const auto& x1 = m_values[min_index].first;
            const auto& y1 = m_values[min_index].second;
            const auto& x2 = m_values[min_index + 1].first;
            const auto& y2 = m_values[min_index + 1].second;

            m_previous_input = x1;
            m_previous_index = min_index;

            return y1 + (input_x - x1) * (y2 - y1) / (x2 - x1);
        }

      private:
        T                            m_previous_input{std::numeric_limits<T>::lowest()};
        size_t                       m_previous_index{0};
        std::vector<std::pair<T, T>> m_values;
    };
}