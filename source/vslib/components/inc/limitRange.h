//! @file
//! @brief Defines the Component class for various limits protection.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    template<fgc4::utils::NumericScalar T>
    class LimitRange : public Component
    {
      public:
        LimitRange(std::string_view name, Component* parent = nullptr)
            : Component("LimitRange", name, parent),
              min(*this, "lower_threshold"),
              max(*this, "upper_threshold"),
              dead_zone(*this, "dead_zone")
        {
        }

        //! Checks minimum and maximum thresholds as well as dead_zone and returns the processed value
        //!
        //! @param input Numerical input to be checked
        //! @return Either original input if no issues were found or nearest edge of allowed zone, 0.0 if NaN was
        //! provided
        T limit(T input) noexcept
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return 0.0;   // fixes nan input to return 0.0
                }
            }

            if (m_dead_zone_defined && (input > dead_zone[0] && input < dead_zone[1]))
            {
                return (abs(dead_zone[0] - input) > abs(dead_zone[1] - input)) ? dead_zone[1] : dead_zone[0];
            }

            if (input < min)
            {
                return min;
            }

            if (input > max)
            {
                return max;
            }

            return input;
        }

        //! Checks minimum and maximum thresholds as well as dead_zone
        //!
        //! @param input Numerical input to be checked
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limitNonRT(T input) noexcept
        {
            if (std::isnan(input))
            {
                return fgc4::utils::Warning("Value is NaN.\n");
            }

            if (m_dead_zone_defined && (input > dead_zone[0] && input < dead_zone[1]))
            {
                return fgc4::utils::Warning(fmt::format(
                    "Value: {} is inside the defined dead zone of [{}, {}].\n", input, dead_zone[0], dead_zone[1]
                ));
            }

            if (input < min)
            {
                return fgc4::utils::Warning(fmt::format("Value: {} is below the minimal value of {}.\n", input, min));
            }
            if (input > max)
            {
                return fgc4::utils::Warning(fmt::format("Value: {} is above the maximal value of {}.\n", input, max));
            }

            return {};
        }

        Parameter<T>                min;
        Parameter<T>                max;
        Parameter<std::array<T, 2>> dead_zone;

        //! Verifies parameters after any Parameter has been modified
        //!
        //! @return Optionally returns a warning if any issues have been found
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_dead_zone_defined = (dead_zone.toValidate()[0] != dead_zone.toValidate()[1]);

            if (m_dead_zone_defined && (dead_zone.toValidate()[0] > dead_zone.toValidate()[1]))
            {
                return fgc4::utils::Warning("Upper edge of the dead_zone is below the lower edge.\n");
            }

            if (min.toValidate() >= max.toValidate())
            {
                return fgc4::utils::Warning("Attempted to set the lower limit below the upper limit.\n");
            }

            return {};
        }

      private:
        bool m_dead_zone_defined{false};
    };
}   // namespace vslib