//! @file
//! @brief Defines the Component class for a look-up table with periodic boundary conditions.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.hpp"
#include "containerSearch.hpp"
#include "lookupTable.hpp"
#include "parameter.hpp"
#include "typeTraits.hpp"

namespace vslib
{
    template<fgc4::utils::NumericScalar IndexType, fgc4::utils::NumericScalar StoredType = IndexType>
    class PeriodicLookupTable : public LookupTable<IndexType, StoredType>
    {
      public:
        //! Constructor for PeriodicLookupTable Component.
        //!
        //! @param name Name of the PeriodicLookupTable component object
        //! @param parent Pointer to the parent of this table
        //! @param values Vector with lookup table index-value pairs
        //! @param equal_binning Flag signalling whether the lookup table indexing has equal spaced binning
        PeriodicLookupTable(
            std::string_view name, Component& parent, std::vector<std::pair<IndexType, StoredType>>&& values,
            const bool equal_binning = false
        ) noexcept
            : LookupTable<IndexType, StoredType>(name, parent, std::move(values), equal_binning)
        {
            m_span_x = this->m_upper_edge_x - this->m_lower_edge_x;

            if constexpr (fgc4::utils::Integral<IndexType>)
            {
                m_span_x = abs(m_span_x);
            }
            else
            {
                m_span_x = fabs(m_span_x);
            }
        }

        //! Provides an interpolated y-axis value from the stored values closest to the provided x-axis input.
        //!
        //! @param input_x X-axis input value
        //! @param random_access Switch informing if the input_x is coming linearly or randomly, allows for binary
        //! search optimisation in the latter case
        //! @return Y-axis value result of the interpolation
        [[nodiscard]] StoredType interpolate(IndexType input_x, const bool random_access = false)
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
        IndexType m_span_x;   //!< Range of x values covered by the held data table
    };

}   // namespace vslib
