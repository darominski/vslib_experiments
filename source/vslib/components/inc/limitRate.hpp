//! @file
//! @brief Defines the Component class for rate limit protection.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <limits>
#include <string>

#include "component.hpp"
#include "parameter.hpp"

namespace vslib
{
    template<fgc4::utils::NumericScalar T>
    class LimitRate : public Component
    {
      public:
        //! Constructor for the LimitRate component, initializes change_rate Parameter.
        //!
        //! @param name Name of this Limit Component
        //! @param parent Parent of this Limit Component
        LimitRate(std::string_view name, Component& parent)
            : Component("LimitRate", name, parent),
              change_rate(*this, "change_rate")
        {
            static_assert(!std::is_unsigned_v<T>, "Unsigned integer is not a safe type to use for a rate of change.");
        }

        //! Checks the input against the maximal rate of change.
        //!
        //! @param input Numerical input to be checked
        //! @param time_difference Time difference between function calls
        //! @return Either original input if no issues were found or maximal allowed rate, 0.0 if NaN was provided or
        //! time_difference is 0
        [[nodiscard]] T limit(T input, const double time_difference)
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return std::numeric_limits<T>::min();
                }
            }

            if (time_difference == 0)
            {
                return std::numeric_limits<T>::max();   // no meaningful rate can be calculated
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
            const double rate = fabs(input - m_previous_value) / time_difference;
            if (rate > change_rate)
            {
                // maximal input to not violate the rate of change
                input = m_previous_value + change_rate * time_difference;

                m_previous_value = input;
                return input;
            }

            m_previous_value = input;

            return input;
        }

        //! Resets the component to the initial state of the previous_value.
        void reset() noexcept
        {
            m_previous_value     = T{};
            m_previous_value_set = false;
        }

        Parameter<T> change_rate;   //!< Maximal allowed rate of change

      private:
        T m_previous_value{T{}};   //!< Holds previous input value

        bool m_previous_value_set{false};   //!< Flag informing whether the previous value has been set
    };
}   // namespace vslib