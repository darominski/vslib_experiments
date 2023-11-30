//! @file
//! @brief Defines class for a finite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "component.h"

namespace vslib
{
    template<int32_t BufferLength>
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
            shiftBuffer(input);
            return (m_cumulative - m_buffer[m_front - 1] + m_buffer[m_front]) / m_filtered_counter;
        }

      private:
        std::array<double, BufferLength> m_buffer{0};
        int32_t                          m_filtered_counter{0};
        int32_t                          m_front{BufferLength - 1};
        double                           m_cumulative{0.0};

        //! Pushes the provided value into the front of the buffer and removes the oldest value
        //!
        //! @param input Input value to be added to the front of the buffer
        void shiftBuffer(double input)
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
