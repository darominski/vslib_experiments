//! @file
//! @brief Defines class for a finite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "filter.h"
#include "parameter.h"

namespace vslib
{
    template<int64_t FilterOrder>
    class FIRFilter : public Filter
    {
        constexpr static int64_t buffer_length = FilterOrder + 1;

      public:
        //! Constructor of the FIR filter component, initializing one Parameter: coefficients
        FIRFilter(std::string_view name, Component* parent)
            : Filter("FIRFilter", name, parent),
              coefficients(*this, "coefficients")
        {
            static_assert(FilterOrder >= 1, "Filter order needs to be a positive number larger than zero.");
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            shiftBuffer(input);
            double output(0);

            for (int64_t index = 0; index < buffer_length; index++)
            {
                int64_t buffer_index = (m_head - 1 - index);
                // Benchmarking showed a significant speed-up (>30% for orders higher than 2)
                // when if statement is used instead of modulo to perform the shift below
                // tertiary operator does not improve the efficiency by more than 2% at a cost to readability
                if (buffer_index < 0)
                {
                    buffer_index += buffer_length;
                }
                output += m_buffer[buffer_index] * coefficients[index];
            }

            return output;
        }

        //! Filters the provided input array by convolving coefficients and the input.
        //!
        //! @param input Input values to be filtered
        //! @return Filtered values
        template<size_t N>
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

        Parameter<std::array<double, buffer_length>> coefficients;

        //! Copies Parameter values into the local container for optimised access
        //!
        //! @return Optionally returns a Warning if an issue was found
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            std::copy(coefficients.toValidate().cbegin(), coefficients.toValidate().cend(), m_coefficients.begin());
            return {};
        }

      private:
        std::array<double, buffer_length> m_coefficients{0};
        std::array<double, buffer_length> m_buffer{0};
        int64_t                           m_head{0};

        //! Pushes the provided value into the front of the buffer and removes the oldest value
        //!
        //! @param input Input value to be added to the front of the buffer
        void shiftBuffer(double input)
        {
            m_buffer[m_head] = input;

            m_head++;
            if (m_head >= buffer_length)
            {
                m_head -= buffer_length;
            }
        }
    };

}   // namespace vslib
