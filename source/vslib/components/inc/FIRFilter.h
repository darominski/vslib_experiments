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
    class FIRFilter : public Component
    {
      public:
        //! Constructor of the low pass filter component, initializing one Parameter: coefficients
        FIRFilter(std::string_view name, Component* parent = nullptr)
            : Component("FIRFilter", name, parent),
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
                output += coefficients[index] * m_buffer[(index + m_front + 1) % N];
            }
            return output;
        }

        //! Filters the provided input array by convolving coefficients and the input.
        //!
        //! @param input Input values to be filtered
        //! @return Filtered values
        std::array<double, N> filter(const std::array<double, N>& inputs)
        {
            m_buffer = inputs;
            std::array<double, N> outputs{0};
            for (int64_t index = 0; index < N; index++)
            {
                const auto& input = inputs[index];
                outputs[index]    = std::accumulate(
                    coefficients.cbegin(), coefficients.cend(), 0.0,
                    [&input](const auto& lhs, const auto& coefficient)
                    {
                        return lhs + input * coefficient;
                    }
                );
            }
            return outputs;
        }

        Parameter<std::array<double, N>> coefficients;

      private:
        std::array<double, N> m_buffer{0};
        int64_t               m_front = N - 1;

        //! Pushes the provided value into the front of the buffer and removes the oldest value
        //!
        //! @param input Input value to be added to the front of the buffer
        void shiftBuffer(double input)
        {
            m_buffer[m_front] = input;
            m_front--;
            if (m_front < 0)
            {
                m_front = N - 1;
            }
        }
    };
}   // namespace vslib
