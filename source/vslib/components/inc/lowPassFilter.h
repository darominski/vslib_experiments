//! @file
//! @brief Defines class for a low-pass filter.
//! @author Dominik Arominski

#pragma once

#include <deque>
#include <string>

#include "component.h"
#include "ringBufferAllocator.h"

namespace vslib::components
{

    using DequeAllocator = fgc4::utils::StaticRingBufferAllocator<double, std::deque<double>, 1000>;

    using Deque = std::deque<double, DequeAllocator>;

    template<size_t N>
    class LowPassFilter : public Component
    {
      public:
        LowPassFilter(std::string_view name, Component* parent = nullptr)
            : Component("LowPassFilter", name, parent),
              coefficients(*this, "coefficients"),
              m_buffer(N, 0.0)
        {
        }

        double filter(double input)
        {
            shiftBuffer(input);
            double output = 0.0;
            for (int index = 0; index < N; index++)
            {
                output = m_buffer[index] * coefficients[index];
            }
            return output;
        }

        parameters::Parameter<std::array<double, N>> coefficients;

      private:
        Deque m_buffer;

        void shiftBuffer(double input)
        {
            // for (size_t index = N - 1; index > 0; --index) {
            //     buffer[index] = buffer[index - 1];
            // }
            // m_buffer[0] = input;
            m_buffer.push_front(input);
            m_buffer.pop_back();
        }
    };

}