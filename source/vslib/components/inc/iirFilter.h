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
    template<uint64_t BufferLength>
    class IIRFilter : public Filter
    {
      public:
        //! Constructor of the IIR filter component, initializing one Parameter: coefficients
        IIRFilter(std::string_view name, Component* parent = nullptr)
            : Filter("IIRFilter", name, parent),
              numerator(*this, "numerator_coefficients"),
              denominator(*this, "denominator_coefficients")
        {
            static_assert(BufferLength > 1, "Buffer length needs to be a positive number larger than one.");
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //! and previously filtered values.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            shiftInputBuffer(input);
            double output = m_inputs_buffer[m_head] * numerator[0];

            for (uint64_t index = 1; index < BufferLength; index++)
            {
                uint64_t buffer_index = (index + m_head);
                // Benchmarking showed a significant speed-up (>30% for orders higher than 2)
                // when if statement is used instead of modulo to perform the shift below
                if (buffer_index >= BufferLength)
                {
                    buffer_index -= BufferLength;
                }
                output += m_inputs_buffer[buffer_index] * numerator[index]
                    - m_outputs_buffer[buffer_index] * denominator[index];
            }

            shiftOutputBuffer(output);

            return output;
        }

        //! Filters the provided input array by filtering each element of the input.
        //!
        //! @param input Array with input values to be filtered
        //! @return Array with the filtered values
        template<uint64_t N>
        std::array<double, N> filter(const std::array<double, N>& inputs)
        {
            std::array<double, N> outputs{0};
            std::transform(
                inputs.cbegin(), inputs.cend(), outputs.begin(),
                [&](const auto& input)
                {
                    return filter(input);
                }
            );

            return outputs;
        }

        Parameter<std::array<double, BufferLength>> numerator;
        Parameter<std::array<double, BufferLength>> denominator;

      private:
        std::array<double, BufferLength> m_inputs_buffer{0};
        std::array<double, BufferLength> m_outputs_buffer{0};
        int64_t                          m_head{BufferLength - 1};

        //! Pushes the provided value into the front of the buffer, overriding the oldest value in effect
        //!
        //! @param input Input value to be added to the front of the inputs buffer
        void shiftInputBuffer(double input)
        {
            m_inputs_buffer[m_head] = input;
        }

        //! Pushes the provided value into the front of the output buffer, overriding the oldest value in effect
        //!
        //! @param output Output value to be added to the front of the outputs buffer
        void shiftOutputBuffer(double output)
        {
            m_outputs_buffer[m_head] = output;
            m_head--;
            if (m_head < 0)
            {
                m_head = BufferLength - 1;
            }
        }
    };

    // ************************************************************
    // Partial template specialization for the first-order filter
    //
    // Benchmarking showed 19% gain for the first order, and only 4% for the 2nd order. Therefore, only
    // the first order is specialized.

    template<>
    class IIRFilter<2> : public Filter
    {
      public:
        //! Constructor of the first-order IIR filter component, initializing one Parameter: coefficients
        IIRFilter(std::string_view name, Component* parent = nullptr)
            : Filter("IIRFirstOrderFilter", name, parent),
              numerator(*this, "numerator_coefficients"),
              denominator(*this, "denominator_coefficients")
        {
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //! and previously filtered value's output.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            double const output
                = input * numerator[0] + m_previous_input * numerator[1] - m_previous_output * denominator[1];

            m_previous_input  = input;
            m_previous_output = output;

            return output;
        }

        Parameter<std::array<double, 2>> numerator;
        Parameter<std::array<double, 2>> denominator;

      private:
        double m_previous_input{0};
        double m_previous_output{0};
    };
}   // namespace vslib
