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
            : Component("Limit", name, parent),
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
            integral_buffer[m_head_integral] = input;
            m_head_integral++;
            if (m_head_integral >= integral_limit_window_length)
            {
                m_head_integral -= integral_limit_window_length;
            }

            if (std::accumulate(
                    std::cbegin(integral_buffer), std::cbegin(integral_buffer) + integral_limit_window_length, 0
                )
                >= integral_limit)
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
            m_head_integral = 0;
            std::fill(std::begin(integral_buffer), std::end(integral_buffer), 0);
        }

        Parameter<T>      integral_limit;
        Parameter<size_t> integral_limit_window_length;

      private:
        int64_t m_head_integral{0};

        std::array<T, TimeWindowLength> integral_buffer{0};
    };
}   // namespace vslib