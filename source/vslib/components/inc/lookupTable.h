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
        LookupTable(std::string_view name, Component* parent, std::vector<std::pair<T, T>>& values)
            : Component("LookupTable", name, parent),
              m_values{values}
        {
        }

        //! For provided x-axis input provides an interpolated y-axis value from the stored values
        //!
        //! @param input_x X-axis input value to interpolate
        //! @return Y-axis value result of the interpolation
        T interpolate(T input_x) const noexcept
        {
            T      min_value{std::numeric_limits<T>::max()};
            size_t min_index{m_values.size()};
            for (size_t index = 0; const auto& point : m_values)
            {
                auto const diff = abs(point.first - input_x);
                if (min_value > diff)
                {
                    min_value = diff;
                    min_index = index;
                }
            }
            if (min_index > (m_values.size() - 1))
            {
                fgc4::utils::Warning(fmt::format(
                    "Interpolation error: provided input value: {} outside of provided look-up table bounds of: [{}, "
                    "{}]",
                    input_x, m_values[0].first, m_values[m_values.size() - 1].first
                ));
                return T{};
            }

            const auto& x1 = m_values[min_index].first;
            const auto& y1 = m_values[min_index].second;
            const auto& x2 = m_values[min_index + 1].first;
            const auto& y2 = m_values[min_index + 1].second;
            return y1 + (input_x - x1) * (y2 - y1) / (x2 - x1);
        }

      private:
        std::vector<std::pair<T, T>> m_values;
    };
}