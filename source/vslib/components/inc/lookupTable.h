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
        LookupTable(
            std::string_view name, Component* parent, std::vector<std::pair<IndexType, StoredType>>&& values,
            bool equal_binning = false
        ) noexcept
            : Component("LookupTable", name, parent),
              m_bin_size{values[1].first - values[0].first},
              m_values{std::move(values)},
              m_equal_binning(equal_binning)
        {
            assert(m_values.size() >= 1);
            m_lower_edge_x          = m_values[0].first;
            m_upper_edge_x          = m_values[m_values.size() - 1].first;
            m_previous_section_x[0] = m_lower_edge_x;
            m_previous_section_x[1] = m_lower_edge_x;
        }

        //! For provided x-axis input provides an interpolated y-axis value from the stored values
        //!
        //! @param input_x X-axis input value to interpolate
        //! @param random_access Switch informing if the input_x is coming linearly or randomly, allows for binary
        //! search optimisation in the latter case
        //! @return Y-axis value result of the interpolation
        StoredType interpolate(IndexType input_x, bool random_access = false) noexcept
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

            size_t     start_loop_index = 0;
            IndexType  x1, x2;
            StoredType y1, y2;

            if (input_x >= m_previous_section_x[0])
            {
                if (input_x <= m_previous_section_x[1])   // same section
                {
                    return m_previous_section_y + (input_x - m_previous_section_x[1]) * m_interpolation_factor;
                }   // else: new section and we need to find new edges
                start_loop_index = m_previous_section_index;
            }

            if (m_equal_binning)
            {
                // Going branchless with constexpr if and placing m_equal_binning in the template does not provide any
                // benefit in this case

                // This case provides a 15% speedup for a 100-element lookup table when compared with linear
                // time monotonic access from the 'else' case.

                index_search(input_x, x1, y1, x2, y2);
            }
            else
            {
                // Existence of this branch leads to a loss of 1% performance.

                // binary_search performs a binary search, more efficient with random access, while
                // for monotonic access the linear linear_search should be more efficient assuming that the next
                // point is relatively close to the previously interpolated one.
                random_access ? binary_search(input_x, start_loop_index, x1, y1, x2, y2)
                              : linear_search(input_x, start_loop_index, x1, y1, x2, y2);
            }

            m_previous_section_y    = y1;
            m_previous_section_x[1] = x1;
            m_previous_section_x[0] = x2;
            m_interpolation_factor  = (y2 - y1) / (x2 - x1);

            return y1 + (input_x - x1) * m_interpolation_factor;
        }

        //! Provides random-access operator overload to the index-th element of the stored lookup table
        //!
        //! @param index Index of the element to be returned
        //! @return Y-value of the function at the index
        const StoredType& operator[](size_t index) const
        {
            return m_values[index].second;
        }

        //! Resets the Component to its initial state
        void reset() noexcept
        {
            m_previous_section_x[0]  = m_lower_edge_x;
            m_previous_section_x[1]  = m_lower_edge_x;
            m_previous_section_y     = m_values[0].second;
            m_previous_section_index = 0;
        }

      private:
        std::array<IndexType, 2> m_previous_section_x;   //! Edges of the previous section
        StoredType               m_previous_section_y;   //! Function's value at the upper edge of the previous section

        size_t     m_previous_section_index{0};
        StoredType m_interpolation_factor;

        IndexType m_lower_edge_x;
        IndexType m_upper_edge_x;

        const IndexType m_bin_size{0};

        std::vector<std::pair<IndexType, StoredType>> m_values;

        const bool m_equal_binning{false};

        void index_search(IndexType input_x, IndexType& x1, StoredType& y1, IndexType& x2, StoredType& y2)
        {
            const int64_t position = static_cast<int64_t>((input_x - m_lower_edge_x) / m_bin_size);
            x1                     = m_values[position].first;
            y1                     = m_values[position].second;
            x2                     = m_values[position + 1].first;
            y2                     = m_values[position + 1].second;
        }

        void linear_search(
            IndexType input_x, size_t start_index, IndexType& x1, StoredType& y1, IndexType& x2, StoredType& y2
        )
        {
            const auto& it = std::find_if(
                m_values.cbegin() + start_index, m_values.cend(),
                [&input_x](const auto& point)
                {
                    return point.first >= input_x;
                }
            );
            m_previous_section_index = std::distance(m_values.cbegin(), it);
            x1                       = (it - 1)->first;
            y1                       = (it - 1)->second;
            x2                       = it->first;
            y2                       = it->second;
        }

        void binary_search(
            IndexType input_x, size_t start_index, IndexType& x1, StoredType& y1, IndexType& x2, StoredType& y2
        )
        {
            const auto& it = std::lower_bound(
                m_values.cbegin() + start_index, m_values.cend(), input_x,
                [](const auto& point, const auto& input)
                {
                    return point.first <= input;
                }
            );
            m_previous_section_index = std::distance(m_values.cbegin(), it);
            x1                       = (it - 1)->first;
            y1                       = (it - 1)->second;
            x2                       = it->first;
            y2                       = it->second;
        }
    };

}   // namespace vslib
