//! @file
//! @brief Defines the Component class for a look-up table with periodic boundary conditions.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "containerSearch.h"
#include "parameter.h"
#include "typeTraits.h"

namespace vslib
{
    template<fgc4::utils::NumericScalar IndexType, fgc4::utils::NumericScalar StoredType = IndexType>
    class PeriodicLookupTable : public Component
    {
      public:
        //! Defines constructor for PeriodicLookupTable component
        //!
        //! @param name Name of the PeriodicLookupTable component object
        //! @param parent Pointer to the parent of this table
        //! @param values Vector with x-y pairs of the function to be stored
        PeriodicLookupTable(std::string_view name, Component* parent) noexcept
            : Component("PeriodicLookupTable", name, parent)
        {
        }

        //! For provided x-axis input provides an interpolated y-axis value from the stored values
        //!
        //! @param input_x X-axis input value to interpolate
        //! @param random_access Switch informing if the input_x is coming linearly or randomly, allows for binary
        //! search optimisation in the latter case
        //! @return Y-axis value result of the interpolation
        StoredType interpolate(IndexType input_x, bool random_access = false) noexcept
        {
            // handle interpolation saturation cases: bring back the value to inside the range
            if (input_x < m_lower_edge_x || input_x > m_upper_edge_x)
            {
                input_x = std::fmod(input_x - m_lower_edge_x, m_span_x);
                if (input_x < 0)
                {
                    input_x += m_span_x;
                }
                input_x += m_lower_edge_x;
            }

            size_t     start_loop_index = 0;
            IndexType  x1, x2;
            StoredType y1, y2;

            if (input_x >= m_previous_section_x[0])
            {
                if (input_x < m_previous_section_x[1])   // same section
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

                utils::index_search(m_values, input_x, m_lower_edge_x, m_bin_size, x1, y1, x2, y2);
            }
            else
            {
                // Existence of this branch leads to a loss of 1% performance.

                // binary_search performs a binary search, more efficient with random access, while
                // for monotonic access the linear linear_search should be more efficient assuming that the next
                // point is relatively close to the previously interpolated one.
                m_previous_section_index
                    = random_access ? utils::binary_search(m_values, input_x, start_loop_index, x1, y1, x2, y2)
                                    : utils::linear_search(m_values, input_x, start_loop_index, x1, y1, x2, y2);
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

        //! Sets the provided data table to the internal values
        //!
        //! @param data Vector of index-value pairs to be set as lookup table
        //! @param equal_binning Flag to signal that the provided data has constant bin spacing
        void setData(std::vector<std::pair<IndexType, StoredType>>&& data, bool equal_binning = false) noexcept
        {
            assert(data.size() >= 1);

            m_lower_edge_x          = data[0].first;
            m_upper_edge_x          = data[data.size() - 1].first;
            m_span_x                = abs(m_upper_edge_x - m_lower_edge_x);
            m_previous_section_x[0] = m_lower_edge_x;
            m_previous_section_x[1] = m_lower_edge_x;

            m_bin_size = data[1].first - data[0].first;

            m_equal_binning = equal_binning;
            m_values        = std::move(data);
        }

        //! Provides a reference to the data table
        //!
        //! @return Data table of index-value pairs
        auto& getData()
        {
            return m_values;
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
        std::array<IndexType, 2> m_previous_section_x{IndexType{}, IndexType{}};   //! Edges of the previous section
        StoredType               m_previous_section_y;   //! Function's value at the upper edge of the previous section

        size_t     m_previous_section_index{1};
        StoredType m_interpolation_factor;

        IndexType m_lower_edge_x;
        IndexType m_upper_edge_x;
        IndexType m_span_x;

        IndexType m_bin_size{0};

        std::vector<std::pair<IndexType, StoredType>> m_values;

        bool m_equal_binning{false};
    };

}   // namespace vslib
