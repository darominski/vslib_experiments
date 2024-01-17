//! @file
//! @brief Defines class for a second-order box averaging filter.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "filter.h"

namespace vslib
{
    class BoxSecondOrderFilter : public Filter
    {

      public:
        //! Constructor of the box filter component
        BoxSecondOrderFilter(std::string_view name, Component* parent = nullptr)
            : Filter("BoxSecondOrderFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            double const result = input + m_previous_value + m_earlier_value;
            m_earlier_value     = m_previous_value;
            m_previous_value    = input;
            return result / 3.0;
        }

      private:
        double m_previous_value{0};
        double m_earlier_value{0};
    };
}   // namespace vslib
