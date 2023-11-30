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
    class FIRFilter : public Component
    {
      public:
        //! Constructor of the FIR filter component, initializing one Parameter: coefficients
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
            for (int64_t index = 0; index < BufferLength; index++)
            {
                output += coefficients[BufferLength - index - 1] * m_buffer[(index + m_front + 1) % BufferLength];
            }
            return output;
        }

        //! Filters the provided input array by convolving coefficients and the input.
        //!
        //! @param input Input values to be filtered
        //! @return Filtered values
        std::array<double, BufferLength> filter(const std::array<double, BufferLength>& inputs)
        {
            m_buffer = inputs;
            std::array<double, BufferLength> outputs{0};
            for (int64_t index = 0; index < BufferLength; index++)
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

        Parameter<std::array<double, BufferLength>> coefficients;

      private:
        std::array<double, BufferLength> m_buffer{0.0};
        int32_t                          m_front{BufferLength - 1};

        //! Pushes the provided value into the front of the buffer and removes the oldest value
        //!
        //! @param input Input value to be added to the front of the buffer
        void shiftBuffer(double input)
        {
            m_buffer[m_front] = input;
            m_front--;
            if (m_front < 0)
            {
                m_front = BufferLength - 1;
            }
        }
    };
}   // namespace vslib
