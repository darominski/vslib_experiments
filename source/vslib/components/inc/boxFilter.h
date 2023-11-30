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
            auto const input_integer = static_cast<int32_t>(m_float_to_integer * input);
            shiftBuffer(input_integer);
            m_cumulative += m_buffer[m_front] - m_buffer[m_front - 1];
            return m_integer_to_float * m_cumulative / m_filtered_counter;
        }

      private:
        std::array<int32_t, BufferLength> m_buffer{0};
        int32_t                           m_filtered_counter{0};
        int32_t                           m_front{BufferLength - 1};
        int32_t                           m_cumulative{0};

        //! Pushes the provided value into the front of the buffer and removes the oldest value
        //!
        //! @param input Input value to be added to the front of the buffer
        void shiftBuffer(int32_t input)
        {
            m_buffer[m_front] = input;
            m_front--;
            m_filtered_counter = (m_filtered_counter + 1) > BufferLength ? BufferLength : (m_filtered_counter + 1);
            if (m_front < 0)
            {
                m_front = BufferLength - 1;
            }
        }
    };
}   // namespace vslib
