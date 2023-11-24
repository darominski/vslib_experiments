//! @file
//! @brief Defines class for a low-pass filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "component.h"

namespace vslib
{
    template<size_t N>
    class LowPassFilter : public Component
    {
      public:
        //! Constructor of the low pass filter component, initializing one Parameter: coefficients
        LowPassFilter(std::string_view name, Component* parent = nullptr)
            : Component("LowPassFilter", name, parent),
              coefficients(*this, "coefficients")
        {
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input)
        {
            shiftBuffer(input);
            double output = 0.0;
            for (int64_t index = 0; index < N; index++)
            {
                output += coefficients[index] * m_buffer[(index + m_front - 1) % N];
            }
            return output;
        }

        Parameter<std::array<double, N>> coefficients;

      private:
        std::array<double, N> m_buffer{0};
        int64_t               m_front = 0;

        //! Pushes the provided value into the front of the buffer and removes the oldest value
        //!
        //! @param input Input value to be added to the front of the buffer
        void shiftBuffer(double input)
        {
            m_buffer[m_front] = input;
            m_front           = (m_front + 1) % N;
        }
    };
}   // namespace vslib
