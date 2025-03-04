//! @file
//! @brief Defines the Component class for integral limit protection.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.hpp"
#include "parameter.hpp"

namespace vslib
{
    template<fgc4::utils::NumericScalar T, size_t time_window_length = 16>
    class LimitIntegral : public Component
    {
      public:
        //! Constructor of the LimitIntegral Component, initializes limit and window length Parameters.
        //!
        //! @param name Name of this Limit Component
        //! @param parent Parent of this Limit Component
        LimitIntegral(std::string_view name, Component& parent)
            : Component("LimitIntegral", name, parent),
              integral_limit(*this, "integral_limit"),
              integral_limit_window_length(*this, "integral_limit_time_window", 0, time_window_length)
        {
        }

        //! Checks for the cumulative (integral) limit.
        //!
        //! @param input Numerical input to be checked
        //! @return True if the provided value does not violate the integral limit threshold, false otherwise
        [[nodiscard]] bool limit(const T input) noexcept
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return false;
                }
            }

            m_cumulative += input - m_integral_buffer[m_head];

            if (m_cumulative > integral_limit)
            {
                return false;
            }

            m_integral_buffer[m_head] = input;
            m_head++;
            if (m_head == integral_limit_window_length)
            {
                m_head = 0;
            }

            return true;
        }

        //! Resets the component to the initial state of buffers, buffer pointers, and the cumulative value.
        void reset() noexcept
        {
            m_head       = 0;
            m_cumulative = T{};
            std::fill(std::begin(m_integral_buffer), std::end(m_integral_buffer), 0);
        }

        Parameter<T>      integral_limit;                 //!< Cumulative limit
        Parameter<size_t> integral_limit_window_length;   //!< Length of the time window for the cumulative calculation

      private:
        size_t m_head{0};   //!< Points to where the oldest input is

        T m_cumulative{T{}};   //!< Holds cumulative value over the time window

        std::array<T, time_window_length> m_integral_buffer{0};   //!< Container with provided inputs
    };
}   // namespace vslib