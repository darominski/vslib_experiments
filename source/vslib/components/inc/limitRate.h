//! @file
//! @brief Defines the Component class for rate limit protection.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    template<fgc4::utils::NumericScalar T>
    class LimitRate : public Component
    {
      public:
        //! Constructor for the LimitRate component
        //!
        //! @param name Name of the LimitRange component
        //! @param parent Optional parent of the LimitRange component
        LimitRate(std::string_view name, Component* parent)
            : Component("LimitRate", name, parent),
              change_rate(*this, "change_rate")
        {
            static_assert(!std::is_unsigned_v<T>, "Unsigned integer is not a safe type to use for a rate of change.");
        }

        //! Checks the rate of change of the input
        //!
        //! @param input Numerical input to be checked
        //! @param time_difference Time difference between function calls
        //! @return Either original input if no issues were found or maximal allowed rate, 0.0 if NaN was provided or
        //! time_difference is 0
        T limit(T input, double time_difference)
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return 0.0;
                }
            }

            if (time_difference == 0)
            {
                return 0.0;   // no meaningful rate can be calculated
            }

            if (!m_previous_value_set)   // avoids failure at first call to limit
            {
                if constexpr ((std::is_same_v<T, float> || std::is_same_v<T, double>))
                {
                    if (std::isinf(input))
                    {
                        return m_previous_value + change_rate * time_difference;
                    }
                }
                m_previous_value     = input;
                m_previous_value_set = true;
                return input;
            }
            const double rate = abs(input - m_previous_value) / time_difference;
            if (rate > change_rate)
            {
                T tmp = input;
                // maximal input to not violate the rate of change
                input = m_previous_value + change_rate * time_difference;

                m_previous_value = input;
                return input;
            }

            m_previous_value = input;

            return input;
        }

        //! Checks the rate of change of the input
        //!
        //! @param input Numerical input to be checked
        //! @param time_difference Time difference between function calls
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limitNonRT(T input, double time_difference)
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return fgc4::utils::Warning("Value is NaN.\n");
                }
            }

            if (time_difference == 0)
            {
                return fgc4::utils::Warning("Time difference is equal to zero in rate limit calculation.\n");
            }

            if (!m_previous_value_set)   // avoids failure at first call to limit
            {
                if constexpr ((std::is_same_v<T, float> || std::is_same_v<T, double>))
                {
                    if (std::isinf(input))
                    {
                        return fgc4::utils::Warning("Value is inf.\n");
                    }
                }
                m_previous_value     = input;
                m_previous_value_set = true;
                return {};
            }
            const double rate = abs(input - m_previous_value) / time_difference;
            if (rate > change_rate)
            {
                auto const& warning_msg = fgc4::utils::Warning(fmt::format(
                    "Value: {} with rate of {} is above the maximal rate of change of: {}.\n", input, rate, change_rate
                ));
                m_previous_value        = input;
                return warning_msg;
            }
            m_previous_value = input;
            return {};
        }

        //! Resets the component to the initial state of the previous_value
        void reset() noexcept
        {
            m_previous_value     = T{};
            m_previous_value_set = false;
        }

        Parameter<T> change_rate;

      private:
        T m_previous_value{T{}};

        bool m_previous_value_set{false};
    };
}   // namespace vslib