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
    template<fgc4::utils::NumericScalar T, size_t TimeWindowLength = 16>
    class LimitIntegral : public Component
    {
      public:
        LimitIntegral(std::string_view name, Component* parent)
            : Component("LimitIntegral", name, parent),
              integral_limit(*this, "integral_limit"),
              integral_limit_window_length(*this, "integral_limit_time_window", 0, TimeWindowLength)
        {
        }

        //! Checks cumulative (integral) limit
        //!
        //! @param input Numerical input to be checked
        //! @return Either original input if no issues were found or maximal allowed value, 0.0 if NaN was provided
        T limit(T input) noexcept
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return 0.0;   // fixes nan input to return 0.0
                }
            }

            if (m_cumulative + input - m_integral_buffer[m_head] > integral_limit)
            {
                // maximum value not violating the integral limit
                input = integral_limit - (m_cumulative - m_integral_buffer[m_head]);
            }

            m_cumulative += (input - m_integral_buffer[m_head]);

            m_integral_buffer[m_head] = input;
            m_head++;
            if (m_head >= integral_limit_window_length)
            {
                m_head -= integral_limit_window_length;
            }

            return input;
        }

        //! Checks cumulative (integral) limit and produces a warning if a violation was found
        //!
        //! @param input Numerical input to be checked
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limitNonRT(T input) noexcept
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return fgc4::utils::Warning("Value is NaN.\n");
                }
            }

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

        //! Resets the component to the initial state of buffers, buffer pointers, and the cumulative value
        void reset() noexcept
        {
            m_head       = 0;
            m_cumulative = T{};
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