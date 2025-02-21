//! @file
//! @brief Defines class for an infinite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "filter.hpp"
#include "parameter.hpp"

namespace vslib
{
    template<int64_t FilterOrder>
    class IIRFilter : public Filter
    {
        constexpr static int64_t buffer_length = FilterOrder + 1;

      public:
        //! Constructor of the IIR filter Component, initializing two Parameters: numerator and denominator coefficient
        //! arrays.
        //!
        //! @param name Name of this Filter Component
        //! @param parent Parent of this Filter Component
        IIRFilter(std::string_view name, Component& parent)
            : Filter("IIRFilter", name, parent),
              numerator(*this, "numerator_coefficients"),
              denominator(*this, "denominator_coefficients")
        {
            static_assert(FilterOrder >= 1, "Filter order needs to be a positive number larger than zero.");
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //! and previously filtered values.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        [[nodiscard]] double filter(const double input) override
        {
            updateInputBuffer(input);
            double output = m_inputs_buffer[m_head] * numerator[0];

            for (int64_t index = 1; index < buffer_length; index++)
            {
                int64_t buffer_index = (m_head - index);
                // Benchmarking showed a significant speed-up (>30% for orders higher than 2)
                // when if statement is used instead of modulo to perform the shift below
                if (buffer_index < 0)
                {
                    buffer_index += buffer_length;
                }
                output += m_inputs_buffer[buffer_index] * m_numerator[index]
                          - m_outputs_buffer[buffer_index] * m_denominator[index];
            }

            shiftOutputBuffer(output);

            return output;
        }

        //! Filters the provided input array by filtering each element of the input.
        //!
        //! @param input Array with input values to be filtered
        //! @return Array with the filtered values
        template<size_t N>
        [[nodiscard]] std::array<double, N> filter(const std::array<double, N>& inputs)
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

        Parameter<std::array<double, buffer_length>> numerator;     //!< Coefficients applied to inputs
        Parameter<std::array<double, buffer_length>> denominator;   //!< Coefficients applied to outputs

        //! Copies Parameter values into local containers for optimised access.
        //!
        //! @return Optionally returns a Warning if an issue was found
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            std::copy(numerator.toValidate().cbegin(), numerator.toValidate().cend(), std::begin(m_numerator));
            std::copy(denominator.toValidate().cbegin(), denominator.toValidate().cend(), std::begin(m_denominator));
            return {};
        }

      private:
        std::array<double, buffer_length> m_numerator;           //!< Local copy of coefficients applied to inputs
        std::array<double, buffer_length> m_denominator;         //!< Local copy of coefficients applied to outputs
        std::array<double, buffer_length> m_inputs_buffer{0};    //!< History of the provided inputs
        std::array<double, buffer_length> m_outputs_buffer{0};   //!< History of the outputs
        int64_t                           m_head{0};   //!< Points to where is the current head of the history buffers

        //! Pushes the provided value into the front of the buffer, overriding the oldest value in effect.
        //!
        //! @param input Input value to be added to the front of the inputs buffer
        void updateInputBuffer(const double input)
        {
            m_inputs_buffer[m_head] = input;
        }

        //! Pushes the provided value into the front of the output buffer, overriding the oldest value in effect.
        //!
        //! @param output Output value to be added to the front of the outputs buffer
        void shiftOutputBuffer(const double output)
        {
            m_outputs_buffer[m_head] = output;

            m_head++;
            if (m_head >= buffer_length)
            {
                m_head -= buffer_length;
            }
        }
    };

    // ************************************************************
    // Override for the first-order filter. Specialization of functions avoids repetition
    // of the entire class structure.
    //
    // Benchmarking showed 19% gain for the first order, and only 4% for the 2nd order. Therefore, only
    // the first order is specialized.

    //! Filters the provided input array by filtering each element of the input.
    //!
    //! @param input Array with input values to be filtered
    //! @return Array with the filtered values
    template<>
    [[nodiscard]] inline double IIRFilter<1>::filter(const double input)
    {
        auto const previous_input  = m_inputs_buffer[0];
        auto const previous_output = m_outputs_buffer[0];

        double const output
            = input * m_numerator[0] + previous_input * m_numerator[1] - previous_output * m_denominator[1];

        // update input and output buffers
        m_inputs_buffer[0]  = input;
        m_outputs_buffer[0] = output;

        return output;
    }

}   // namespace vslib
