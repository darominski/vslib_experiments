//! @file
//! @brief Defines the base class for filters.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    template<typename T, size_t TimeWindowLength = 1, size_t RMSBufferLength = 1>
    class Limit : public Component
    {
      public:
        Limit(std::string_view name, Component* parent = nullptr)
            : Component("Limit", name, parent),
              min(*this, "lower_threshold"),
              max(*this, "upper_threshold"),
              dead_zone(*this, "dead_zone"),
              integral_limit(*this, "integral_limit"),
              rms(*this, "rms_threshold")
        {
        }

        // Q1: what types should we limit ourselves to? only scalar or arrays should be permitted?
        // Q2: how to optimise the design based on the fact that most of the checks are optional?
        // Q3: how to efficiently check if the parameter has been set and should be used?

        //! Checks minimum and maximum thresholds as well as dead_zone
        std::optional<fgc4::utils::Warning> check_min_max(T input)
        {
            if (m_is_dead_zone_defined && (input >= dead_zone[0] && input <= dead_zone[1]))
            {
                // ???, check which edge we are closer to and then return that?
                return fgc4::utils::Warning(fmt::format(
                    "Value: {} is inside the defined dead zone of [{}, {}]\n", input, dead_zone[0], dead_zone[1]
                ));
            }
            if (input <= min)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} is below or equal to the minimal value of {}\n", input, min)
                );
            }
            if (input >= max)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} is above of equal to the maximal value of {}\n", input, max)
                );
            }

            return {};
        }

        //! Checks cumulative (integral) limits
        //!
        //! @param input Numerical input to be checked
        std::optional<fgc4::utils::Warning> check_integral_limit(T input)
        {
            integral_buffer[m_head_integral] = input;
            if (std::accumulate(std::cbegin(integral_buffer), std::cend(integral_buffer), 0) >= integral_limit)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} leads to overflow of the integral limit of {}\n", input, integral_limit)
                );
            }

            m_head_integral++;
            if (m_head_integral >= TimeWindowLength)
            {
                m_head_integral -= TimeWindowLength;
            }
        }

        //! Checks the RMS limits
        //!
        //! @param input Numerical input to be checked against set RMS limit
        std::optional<fgc4::utils::Warning> check_rms_limit(T input)
        {
            rms_buffer[m_head_rms] = input;
            if (sqrt(
                    std::accumulate(
                        std::cbegin(rms_buffer), std::cend(rms_buffer), 0.0,
                        [](const auto& element)
                        {
                            return pow(element, 2);
                        }
                    )
                    / RMSBufferLength
                )
                >= rms)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} deviates too far from the RMS limit of {}\n", input, rms)
                );
            }
            m_head_rms++;
            if (m_head_rms >= RMSBufferLength)
            {
                m_head_rms -= RMSBufferLength;
            }

            return {};
        }


        std::optional<fgc4::utils::Warning> limit(T input)
            requires fgc4::utils::Numeric<decltype(input)>
        {
            // checking straight min/max thresholds with a possible dead-zone
            auto& maybe_warning = check_min_max(input);
            if (maybe_warning.has_value())
            {
                return maybe_warning.value();
            }

            maybe_warning = check_integral_limit(input);
            if (maybe_warning.has_value())
            {
                return maybe_warning.value();
            }

            maybe_warning = check_rms_limit(input);
            if (maybe_warning.has_value())
            {
                return maybe_warning.value();
            }
            return {};
        }

        Parameter<T>                min;
        Parameter<T>                max;
        Parameter<std::array<T, 2>> dead_zone;
        Parameter<T>                integral_limit;
        Parameter<double>           rms;

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            if (dead_zone.isInitialized() && (dead_zone[0] < dead_zone[1]))
            {
                return fgc4::utils::Warning("Upper edge of the dead_zone is below the lower edge.\n");
            }

            if (dead_zone.isInitialized() && (dead_zone[0] != dead_zone[1]))
            {
                m_is_dead_zone_defined = true;
            }
            else
            {
                m_is_dead_zone_defined = false;
            }

            return {};
        }

      private:
        int64_t m_head_integral{0};
        int64_t m_head_rms{0};

        std::array<T, TimeWindowLength> integral_buffer{0};
        std::array<T, RMSBufferLength>  rms_buffer{0};

        bool m_is_dead_zone_defined{false};
    };
}