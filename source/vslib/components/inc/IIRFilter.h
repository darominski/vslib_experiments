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
    template<int32_t BufferLength>
    class IIRFilter : public Filter
    {
      public:
        //! Constructor of the FIR filter component, initializing one Parameter: coefficients
        IIRFilter(std::string_view name, Component* parent = nullptr, double max_input_value = 1e6)
            : Filter("IIRFilter", name, parent, max_input_value),
              nominator(*this, "nominator_coefficients") denominator(*this, "denominator_coefficients")
        {
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //! and previously filtered values.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            auto const input_integer = static_cast<int32_t>(m_float_to_integer * input);
            shiftInputBuffer(input_integer);
            int32_t output = 0.0;
            for (int64_t index = 0; index < BufferLength; index++)
            {
                output += nominator[index] * m_inputs_buffer[(index + m_front + 1) % BufferLength]
                    - denominator[index] * m_outputs_buffer[(index + m_front + 1) % BufferLength]
            }
            shiftOutputBuffer(input_integer);
            return output * m_integer_to_float;
        }

        //! Filters the provided input array by convolving coefficients and the input.
        //!
        //! @param input Input values to be filtered
        //! @return Filtered values
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

        Parameter<std::array<double, BufferLength>> coefficients;

      private:
        std::array<int32_t, BufferLength> m_inputs_buffer{0};
        std::array<int32_t, BufferLength> m_outputs_buffer{0};
        int32_t                           m_front{BufferLength - 1};

        //! Pushes the provided value into the front of the buffer, overriding the oldest value in effect
        //!
        //! @param input Input value to be added to the front of the inputs buffer
        void shiftInputBuffer(int32_t input)
        {
            m_inputs_buffer[m_front] = input;
            m_front--;
            if (m_front < 0)
            {
                m_front = BufferLength - 1;
            }
        }

        //! Pushes the provided value into the front of the buffer, overriding the oldest value in effect
        //!
        //! @param output Output value to be added to the front of the outputs buffer
        void shiftOutputBuffer(int32_t output)
        {
            m_outputs_buffer[m_front] = output;
        }
    };
}   // namespace vslib
