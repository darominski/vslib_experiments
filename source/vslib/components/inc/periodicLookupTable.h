//! @file
//! @brief Defines the Component class for a look-up table with periodic boundary conditions.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "containerSearch.h"
#include "lookupTable.h"
#include "parameter.h"
#include "typeTraits.h"

namespace vslib
{
    template<fgc4::utils::NumericScalar IndexType, fgc4::utils::NumericScalar StoredType = IndexType>
    class PeriodicLookupTable : public LookupTable<IndexType, StoredType>
    {
      public:
        //! Defines constructor for PeriodicLookupTable component
        //!
        //! @param name Name of the PeriodicLookupTable component object
        //! @param parent Pointer to the parent of this table
        PeriodicLookupTable(std::string_view name, Component* parent) noexcept
            : LookupTable<IndexType, StoredType>(name, parent)
        {
        }

        //! For provided x-axis input provides an interpolated y-axis value from the stored values
        //!
        //! @param input_x X-axis input value to interpolate
        //! @param random_access Switch informing if the input_x is coming linearly or randomly, allows for binary
        //! search optimisation in the latter case
        //! @return Y-axis value result of the interpolation
        StoredType interpolate(IndexType input_x, bool random_access = false)
        {
            // handle interpolation saturation cases: bring back the value to inside the range
            if (input_x < this->m_lower_edge_x || input_x > this->m_upper_edge_x)
            {
                input_x = std::fmod(input_x - this->m_lower_edge_x, m_span_x);
                if (input_x < 0)
                {
                    input_x += m_span_x;
                }
                input_x += this->m_lower_edge_x;
            }
            size_t     start_loop_index = 0;
            IndexType  x1, x2;
            StoredType y1, y2;

            if (input_x >= this->m_previous_section_x[0])
            {
                if (input_x < this->m_previous_section_x[1])   // same section
                {
                    return this->m_previous_section_y
                           + (input_x - this->m_previous_section_x[1]) * this->m_interpolation_factor;
                }   // else: new section and we need to find new edges
                start_loop_index = this->m_previous_section_index;
            }

            if (this->m_equal_binning)
            {
                // Going branchless with constexpr if and placing m_equal_binning in the template does not provide any
                // benefit in this case

                // This case provides a 15% speedup for a 100-element lookup table when compared with linear
                // time monotonic access from the 'else' case.

                utils::index_search(this->m_values, input_x, this->m_lower_edge_x, this->m_bin_size, x1, y1, x2, y2);
            }
            else
            {
                // Existence of this branch leads to a loss of 1% performance.

                // binary_search performs a binary search, more efficient with random access, while
                // for monotonic access the linear linear_search should be more efficient assuming that the next
                // point is relatively close to the previously interpolated one.
                this->m_previous_section_index
                    = random_access ? utils::binary_search(this->m_values, input_x, start_loop_index, x1, y1, x2, y2)
                                    : utils::linear_search(this->m_values, input_x, start_loop_index, x1, y1, x2, y2);
            }
            this->m_previous_section_y    = y1;
            this->m_previous_section_x[1] = x1;
            this->m_previous_section_x[0] = x2;
            this->m_interpolation_factor  = (y2 - y1) / (x2 - x1);

            return y1 + (input_x - x1) * this->m_interpolation_factor;
        }

        //! Sets the provided data table to the internal values
        //!
        //! @param data Vector of index-value pairs to be set as lookup table
        //! @param equal_binning Flag to signal that the provided data has constant bin spacing
        void setData(std::vector<std::pair<IndexType, StoredType>>&& data, bool equal_binning = false) noexcept
        {
            LookupTable<IndexType, StoredType>::setData(std::move(data), equal_binning);
            m_span_x = abs(this->m_upper_edge_x - this->m_lower_edge_x);
        }

      private:
        IndexType m_span_x;
    };

}   // namespace vslib
