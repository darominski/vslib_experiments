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
    template<typename T>
    class LimitRate : public Component
    {
      public:
        //! Constructor for the LimitRate component
        //!
        //! @param name Name of the LimitRange component
        //! @param parent Optional parent of the LimitRange component
        LimitRate(std::string_view name, Component* parent = nullptr)
            : Component("Limit", name, parent),
              change_rate(*this, "change_rate")
        {
        }

        //! Checks the rate of change of the input
        //!
        //! @param input Numerical input to be checked
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limit(T input)
        {
            if (input - m_previous_value > change_rate)
            {
                auto const& warning_msg = fgc4::utils::Warning(fmt::format(
                    "Value: {} with difference of {} is above the maximal rate of change of: {}.\n", input,
                    input - m_previous_value, change_rate
                ));
                m_previous_value        = input;
                return warning_msg;
            }
            m_previous_value = input;
            return {};
        }

        Parameter<T> change_rate;

      private:
        T m_previous_value{};
    }
}   // namespace vslib