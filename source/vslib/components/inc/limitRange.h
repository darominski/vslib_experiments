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
        //! Constructor of LimitRange Component, initializes min, max, and dead_zone Parameters.
        //!
        //! @param name Name of this Limit Component
        //! @param parent Parent of this Limit Component
        LimitRange(std::string_view name, Component* parent)
            : Component("LimitRange", name, parent),
              min(*this, "lower_threshold"),
              max(*this, "upper_threshold"),
              dead_zone(*this, "dead_zone")
        {
        }

        //! Checks minimum and maximum thresholds as well as the dead zone and returns the processed value.
        //!
        //! @param input Numerical input to be checked
        //! @return Either original input if no issues were found or nearest edge of allowed zone, minimum representable
        //! value if NaN was provided
        [[nodiscard]] T limit(T input) noexcept
        {
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
            {
                if (std::isnan(input))
                {
                    return std::numeric_limits<T>::min();   // fixes nan input to return min of the type
                }
            }

            if (m_dead_zone_defined && (input > dead_zone[0] && input < dead_zone[1]))
            {
                if constexpr (std::is_unsigned_v<T>)
                {
                    // abs is not defined for unsigned integers
                    return (input - dead_zone[0] > dead_zone[1] - input) ? dead_zone[1] : dead_zone[0];
                }
                else
                {
                    return (abs(dead_zone[0] - input) > abs(dead_zone[1] - input)) ? dead_zone[1] : dead_zone[0];
                }
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

        Parameter<T>                min;         //!< Minimum allowed value
        Parameter<T>                max;         //!< Maximum allowed value
        Parameter<std::array<T, 2>> dead_zone;   //!< Two edges (min, max) of the dead zone

        //! Verifies parameters after any Parameter has been modified.
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
        bool m_dead_zone_defined{false};   //!< Flag whether the optional dead zone has been defined
    };
}   // namespace vslib