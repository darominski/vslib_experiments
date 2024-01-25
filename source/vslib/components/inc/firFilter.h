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
            static_assert(BufferLength > 1, "Buffer length needs to be a positive number larger than one.");
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
                // Benchmarking showed a significant speed-up (>30% for orders higher than 2)
                // when if statement is used instead of modulo to perform the shift below
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

    // ************************************************************
    // Partial template specialization for low-order filters
    //
    // Benchmarking showed 44% gain for the first order, and 72% for the 2nd order.

    template<>
    class FIRFilter<2> : public Filter
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
        double filter(const double input) override
        {
            double const output = input * coefficients[0] + m_previous_input * coefficients[1];
            m_previous_input    = input;
            return output;
        }

        Parameter<std::array<double, 2>> coefficients;

      private:
        double m_previous_input{0.0};
    };

    // ************************************************************

    template<>
    class FIRFilter<3> : public Filter
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
        double filter(const double input) override
        {
            double const output
                = input * coefficients[0] + m_previous_input * coefficients[1] + m_earlier_input * coefficients[2];
            m_earlier_input  = m_previous_input;
            m_previous_input = input;
            return output;
        }

        Parameter<std::array<double, 3>> coefficients;

      private:
        double m_previous_input{0.0};
        double m_earlier_input{0.0};
    };
}   // namespace vslib
