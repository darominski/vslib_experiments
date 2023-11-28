//! @file
//! @brief Defines class for a finite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "component.h"

namespace vslib
{
    template<int64_t BufferLength>
    class BoxFilter : public Component
    {
      public:
        //! Constructor of the box filter component
        BoxFilter(std::string_view name, Component* parent = nullptr)
            : Component("BoxFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input)
        {
            m_filtered_counter   = m_filtered_counter + 1 > BufferLength ? BufferLength : m_filtered_counter + 1;
            const double average = (m_last_average * (m_filtered_counter - 1) + input) / m_filtered_counter;
            m_last_average       = average;
            return average;
        }

      private:
        std::array<double, BufferLength> m_buffer{0};
        int64_t                          m_filtered_counter{0};
        double                           m_last_average{0.0};
    };
}   // namespace vslib