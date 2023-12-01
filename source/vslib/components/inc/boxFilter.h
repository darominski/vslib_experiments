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
    template<int32_t BufferLength>
    class BoxFilter : public Filter
    {
      public:
        //! Constructor of the box filter component
        BoxFilter(std::string_view name, Component* parent = nullptr, double max_input_value = 1e6)
            : Filter("BoxFilter", name, parent, max_input_value)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            int32_t oldest_value = 0;
            if (m_filtered_counter < BufferLength)   // active only in first pass of the buffer
            {
                m_filtered_counter++;
            }
            else   // active only after the buffer has been filled
            {
                oldest_value = m_buffer[m_front];
            }
            auto const input_integer = static_cast<int32_t>(m_float_to_integer * input);
            m_buffer[m_front]        = input_integer;
            m_cumulative             += input_integer - oldest_value;
            m_front                  = (m_front + 1) % BufferLength;
            return m_integer_to_float * m_cumulative / m_filtered_counter;
        }

      private:
        std::array<int32_t, BufferLength> m_buffer{0};
        int32_t                           m_filtered_counter{0};
        int32_t                           m_front{0};
        int32_t                           m_cumulative{0};
    };
}   // namespace vslib
