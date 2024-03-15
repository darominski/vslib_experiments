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
        LimitRate(std::string_view name, Component* parent = nullptr)
            : Component("LimitRate", name, parent),
              change_rate(*this, "change_rate")
        {
            static_assert(!std::is_unsigned_v<T>, "Unsigned integer is not a safe type to use for a rate of change.");
        }

        //! Checks the rate of change of the input
        //!
        //! @param input Numerical input to be checked
        //! @param time_difference Time difference between function calls
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limit(T input, double time_difference)
        {
            if (time_difference == 0)
            {
                return fgc4::utils::Warning("Time difference is equal to zero in rate limit calculation.\n");
            }

            if (!m_previous_value_set)   // avoids failure at first call to limit
            {
                m_previous_value     = input;
                m_previous_value_set = true;
                return {};
            }
            const double rate = (input - m_previous_value) / time_difference;
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