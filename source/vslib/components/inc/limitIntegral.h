//! @file
//! @brief Defines the Component class for integral limit protection.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    template<typename T, size_t TimeWindowLength = 16>
    class LimitIntegral : public Component
    {
      public:
        LimitIntegral(std::string_view name, Component* parent = nullptr)
            : Component("LimitIntegral", name, parent),
              integral_limit(*this, "integral_limit"),
              integral_limit_window_length(*this, "integral_limit_time_window", 0, TimeWindowLength)
        {
        }

        //! Checks cumulative (integral) limit
        //!
        //! @param input Numerical input to be checked
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limit(T input) noexcept
        {
            m_cumulative += (input - m_integral_buffer[m_head]);

            m_integral_buffer[m_head] = input;
            m_head++;
            if (m_head >= integral_limit_window_length)
            {
                m_head -= integral_limit_window_length;
            }

            if (m_cumulative >= integral_limit)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} leads to overflow of the integral limit of {}.\n", input, integral_limit)
                );
            }

            return {};
        }

        //! Resets the component to the initial state of buffers and buffer pointers
        void reset() noexcept
        {
            m_head = 0;
            std::fill(std::begin(m_integral_buffer), std::end(m_integral_buffer), 0);
        }

        Parameter<T>      integral_limit;
        Parameter<size_t> integral_limit_window_length;

      private:
        int64_t m_head{0};

        T m_cumulative{T{}};

        std::array<T, TimeWindowLength> m_integral_buffer{0};
    };
}   // namespace vslib