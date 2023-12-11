//! @file
//! @brief Defines class for an infinite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "filter.h"
#include "fixedPointType.h"
#include "parameter.h"

namespace vslib
{
    template<size_t BufferLength, unsigned short FixedPointMantissa = 24>
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
            shiftInputBuffer(input);
            FixedPoint<FixedPointMantissa> output = m_inputs_buffer[m_front] * numerator[0];
            for (size_t index = 1; index < BufferLength; index++)
            {
                size_t const buffer_index = (index + m_front) % BufferLength;
                output                    += m_inputs_buffer[buffer_index] * numerator[index]
                    - m_outputs_buffer[buffer_index] * denominator[index];
            }
            shiftOutputBuffer(output);
            return output.toDouble();
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

        [[nodiscard]] auto const getMaxInputValue() const noexcept
        {
            return FixedPoint<FixedPointMantissa>::maximumValue();
        }

        Parameter<std::array<double, BufferLength>> numerator;
        Parameter<std::array<double, BufferLength>> denominator;

      private:
        std::array<FixedPoint<FixedPointMantissa>, BufferLength> m_inputs_buffer{0};
        std::array<FixedPoint<FixedPointMantissa>, BufferLength> m_outputs_buffer{0};
        int32_t                                                  m_front{BufferLength - 1};

        //! Pushes the provided value into the front of the buffer, overriding the oldest value in effect
        //!
        //! @param input Input value to be added to the front of the inputs buffer
        void shiftInputBuffer(double input)
        {
            m_inputs_buffer[m_front] = input;
        }

        //! Pushes the provided value into the front of the output buffer, overriding the oldest value in effect
        //!
        //! @param output Output value to be added to the front of the outputs buffer
        void shiftOutputBuffer(FixedPoint<FixedPointMantissa>& output)
        {
            m_outputs_buffer[m_front] = output;
            m_front--;
            if (m_front < 0)
            {
                m_front = BufferLength - 1;
            }
        }
    };
}   // namespace vslib
