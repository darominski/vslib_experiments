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
    template<uint64_t BufferLength>
    class FIRFilter : public Filter
    {
      public:
        //! Constructor of the FIR filter component, initializing one Parameter: coefficients
        FIRFilter(std::string_view name, Component* parent = nullptr)
            : Filter("FIRFilter", name, parent),
              coefficients(*this, "coefficients")
        {
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            shiftBuffer(input);
            double output(0);
            for (uint64_t index = 0; index < BufferLength; index++)
            {
                uint64_t buffer_index = (index + m_head + 1);
                if (buffer_index >= BufferLength)
                {
                    buffer_index -= BufferLength;
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

        Parameter<std::array<double, BufferLength>> coefficients;

      private:
        std::array<double, BufferLength> m_buffer{0};
        int64_t                          m_head{BufferLength - 1};

        //! Pushes the provided value into the front of the buffer and removes the oldest value
        //!
        //! @param input Input value to be added to the front of the buffer
        void shiftBuffer(double input)
        {
            m_buffer[m_head] = input;
            m_head--;
            if (m_head < 0)
            {
                m_head = BufferLength - 1;
            }
        }
    };
}   // namespace vslib
