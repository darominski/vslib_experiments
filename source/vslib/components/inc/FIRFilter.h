//! @file
//! @brief Defines class for a finite-impulse filter.
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
            FixedPoint<FixedPointMantissa> output(0);
            for (int64_t index = 0; index < BufferLength; index++)
            {
                output += m_buffer[(index + m_front + 1) % BufferLength] * coefficients[index];
            }
            return output.toDouble();
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
        [[nodiscard]] auto const getMaxInputValue() const noexcept
        {
            return FixedPoint<FixedPointMantissa>::maximumValue();
        }

        Parameter<std::array<double, BufferLength>> coefficients;

      private:
        std::array<FixedPoint<FixedPointMantissa>, BufferLength> m_buffer{0};
        int64_t                                                  m_front{BufferLength - 1};

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
