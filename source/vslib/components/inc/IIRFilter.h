//! @file
//! @brief Defines class for an infinite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "filter.h"
#include "parameter.h"

namespace vslib
{
    template<size_t BufferLength>
    class IIRFilter : public Filter
    {
      public:
        //! Constructor of the FIR filter component, initializing one Parameter: coefficients
        IIRFilter(std::string_view name, Component* parent = nullptr)
            : Filter("IIRFilter", name, parent),
              numerator(*this, "numerator_coefficients"),
              denominator(*this, "denominator_coefficients")
        {
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //! and previously filtered values.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            // auto const input_integer = static_cast<int32_t>(m_float_to_integer * input);
            shiftInputBuffer(input);
            double output = numerator[0] * m_inputs_buffer[(m_front + 1) % BufferLength];
            for (size_t index = 1; index < BufferLength; index++)
            {
                output += numerator[index] * m_inputs_buffer[(index + m_front + 1) % BufferLength]
                    - denominator[index] * m_outputs_buffer[(index + m_front) % BufferLength];
            }
            shiftOutputBuffer(output);
            return output;
        }

        //! Filters the provided input array by filtering each element of the input.
        //!
        //! @param input Array with input values to be filtered
        //! @return Array with the filtered values
        template<size_t N>
        std::array<double, N> filter(const std::array<double, N>& inputs)
        {
            std::array<double, N> outputs{0};
            int32_t               index = 0;
            for (const auto& input : inputs)
            {
                outputs[index] = filter(input);
                index++;
            }
            return outputs;
        }

        Parameter<std::array<double, BufferLength>> numerator;
        Parameter<std::array<double, BufferLength>> denominator;

      private:
        std::array<double, BufferLength> m_inputs_buffer{0};
        std::array<double, BufferLength> m_outputs_buffer{0};
        int32_t                          m_front{BufferLength - 1};

        //! Pushes the provided value into the front of the buffer, overriding the oldest value in effect
        //!
        //! @param input Input value to be added to the front of the inputs buffer
        void shiftInputBuffer(double input)
        {
            m_inputs_buffer[m_front] = input;
            m_front--;
            if (m_front < 0)
            {
                m_front = BufferLength - 1;
            }
        }

        //! Pushes the provided value into the front of the output buffer, overriding the oldest value in effect
        //!
        //! @param output Output value to be added to the front of the outputs buffer
        void shiftOutputBuffer(double output)
        {
            m_outputs_buffer[m_front] = output;
        }
    };
}   // namespace vslib
