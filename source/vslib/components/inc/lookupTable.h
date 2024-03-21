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
    template<fgc4::utils::NumericScalar IndexType, fgc4::utils::NumericScalar StoredType = IndexType>
    class LookupTable : public Component
    {
      public:
        //! Defines constructor for LookupTable component
        //!
        //! @param name Name of the LookupTable component object
        //! @param parent Pointer to the parent of this table
        //! @param values Vector with x-y pairs of the function to be stored
        LookupTable(std::string_view name, Component* parent, std::vector<std::pair<IndexType, StoredType>>&& values)
            : Component("LookupTable", name, parent),
              m_values{std::move(values)}
        {
            assert(m_values.size() >= 1);
            m_lower_edge_x = m_values[0].first;
            m_upper_edge_x = m_values[m_values.size() - 1].first;
        }

        //! For provided x-axis input provides an interpolated y-axis value from the stored values
        //!
        //! @param input_x X-axis input value to interpolate
        //! @return Y-axis value result of the interpolation
        StoredType interpolate(IndexType input_x) noexcept
        {
            // handle interpolation saturation cases: return the function value at the edge in case of under or overflow
            if (input_x <= m_lower_edge_x)
            {
                return m_values[0].second;
            }

            if (input_x >= m_upper_edge_x)
            {
                return m_values[m_values.size() - 1].second;
            }

            size_t start_loop_index = 0;
            if (input_x >= m_previous_section_x)
            {
                start_loop_index = m_previous_section_x;
            }

            size_t min_index;
            for (size_t index = start_loop_index; index < m_values.size(); index++)
            {
                if (m_values[index].first >= input_x)   // assumes monotonic x-axis distribution
                {
                    min_index = index;
                    break;
                }
            }

            const auto& x1 = m_values[min_index].first;
            const auto& y1 = m_values[min_index].second;
            const auto& x2 = m_values[min_index + 1].first;
            const auto& y2 = m_values[min_index + 1].second;

            m_previous_section_x = x1;
            m_previous_section_y = y1;

            // const auto& it = std::find_if(m_values.cbegin(), m_values.cend(), [&input_x](const auto& point){return
            // input_x >= point.first;});

            // const auto& x1 = it->first;
            // const auto& y1 = it->second;
            // const auto& x2 = (it+1)->first;
            // const auto& y2 = (it+1)->second;
            // m_previous_section_x     = x1;
            // m_previous_section_y     = y1;

            return y1 + (input_x - x1) * (y2 - y1) / (x2 - x1);
        }

      private:
        IndexType  m_previous_section_x;
        StoredType m_previous_section_y;

        IndexType m_lower_edge_x;
        IndexType m_upper_edge_x;

        std::vector<std::pair<IndexType, StoredType>> m_values;
    };
}