//! @file
//! @brief Defines class for a finite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "filter.h"

namespace vslib
{
    class BoxFirstOrderFilter : public Filter
    {

      public:
        //! Constructor of the box filter component
        BoxFirstOrderFilter(std::string_view name, Component* parent = nullptr)
            : Filter("BoxFirstOrderFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            double const result = input + m_previous_value;
            m_previous_value    = input;
            return 0.5 * result;
        }

      private:
        double m_previous_value{0};
    };
}   // namespace vslib
