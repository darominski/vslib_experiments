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
        //! @param values Vector with lookup table index-value pairs
        //! @param equal_binning Flag signalling whether the lookup table indexing has equal spaced binning
        PeriodicLookupTable(
            std::string_view name, Component* parent, std::vector<std::pair<IndexType, StoredType>>&& values,
            bool equal_binning = false
        ) noexcept
            : LookupTable<IndexType, StoredType>(name, parent, std::move(values), equal_binning)
        {
            m_span_x = abs(this->m_upper_edge_x - this->m_lower_edge_x);
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

            return this->interpolate_data(input_x, random_access);
        }

      private:
        IndexType m_span_x;
    };

}   // namespace vslib
