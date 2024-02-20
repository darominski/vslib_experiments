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
    template<typename T, size_t TimeWindowLength = 16, size_t RMSBufferLength = 16>
    class Limit : public Component
    {
      public:
        Limit(std::string_view name, Component* parent = nullptr)
            : Component("Limit", name, parent),
              min(*this, "lower_threshold"),
              max(*this, "upper_threshold"),
              dead_zone(*this, "dead_zone"),
              change_rate(*this, "change_rate"),
              integral_limit(*this, "integral_limit"),
              integral_limit_window_length(*this, "integral_limit_time_window", 0, TimeWindowLength),
              rms(*this, "rms_threshold"),
              rms_time_constant(*this, "rms_time_constant", 0, RMSBufferLength)
        {
        }

        // Q1: what types should we limit ourselves to? only scalar or arrays should be permitted?
        // Q2: how to optimise the design based on the fact that most of the checks are optional?
        // Q3: how to efficiently check if the parameter has been set and should be used?

        //! Checks minimum and maximum thresholds as well as dead_zone
        //!
        //! @param input Numerical input to be checked
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> check_min_max_limit(T input) noexcept
        {
            if (m_dead_zone_defined && (input >= dead_zone[0] && input <= dead_zone[1]))
            {
                // ???, check which edge we are closer to and then return that?
                return fgc4::utils::Warning(fmt::format(
                    "Value: {} is inside the defined dead zone of [{}, {}].\n", input, dead_zone[0], dead_zone[1]
                ));
            }

            if (input <= min)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} is below or equal to the minimal value of {}.\n", input, min)
                );
            }
            if (input >= max)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} is above of equal to the maximal value of {}.\n", input, max)
                );
            }

            return {};
        }

        //! Checks the rate of change of the input
        //!
        //! @param input Numerical input to be checked
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> check_change_rate_limit(T input)
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

        //! Checks cumulative (integral) limits
        //!
        //! @param input Numerical input to be checked
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> check_integral_limit(T input) noexcept
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

        //! Checks the RMS limits
        //!
        //! @param input Numerical input to be checked against set RMS limit
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> check_rms_limit(T input) noexcept
        {
            rms_buffer[m_head_rms] = input;
            m_head_rms++;
            if (m_head_rms >= rms_time_constant)
            {
                m_head_rms -= rms_time_constant;
            }

            if (sqrt(
                    std::accumulate(
                        std::begin(rms_buffer), std::begin(rms_buffer) + rms_time_constant, 0.0,
                        [](auto lhs, auto const& element)
                        {
                            return lhs + pow(element, 2);
                        }
                    )
                    / RMSBufferLength
                )
                >= rms)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} deviates too far from the RMS limit of {}.\n", input, rms)
                );
            }

            return {};
        }

        //! Checks all the specified limits
        //!
        //! @param input Value to be checked against all specified limits
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limit(T input) noexcept
            requires fgc4::utils::Numeric<decltype(input)>
        {
            // checking straight min/max thresholds with a possible dead-zone
            const auto& maybe_warning_min_max = check_min_max_limit(input);
            if (maybe_warning_min_max.has_value())
            {
                return maybe_warning_min_max.value();
            }

            const auto& maybe_warning_change_rate = check_change_rate_limit(input);
            if (maybe_warning_change_rate.has_value())
            {
                return maybe_warning_change_rate.value();
            }

            const auto& maybe_warning_integral = check_integral_limit(input);
            if (maybe_warning_integral.has_value())
            {
                return maybe_warning_integral.value();
            }

            const auto& maybe_warning_rms = check_rms_limit(input);
            if (maybe_warning_rms.has_value())
            {
                return maybe_warning_rms.value();
            }

            return {};
        }

        //! Resets the component to the initial state of buffers and buffer pointers
        void reset() noexcept
        {
            m_head_integral = 0;
            m_head_rms      = 0;
            std::fill(std::begin(integral_buffer), std::end(integral_buffer), 0);
            std::fill(std::begin(rms_buffer), std::end(rms_buffer), 0);
        }

        Parameter<T>                min;
        Parameter<T>                max;
        Parameter<std::array<T, 2>> dead_zone;
        Parameter<T>                change_rate;
        Parameter<T>                integral_limit;
        Parameter<size_t>           integral_limit_window_length;
        Parameter<double>           rms;
        Parameter<size_t>           rms_time_constant;

        //! Verifies parameters after any Parameter has been modified
        //!
        //! @return Optionally returns a warning if any issues have been found
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            if (dead_zone.isInitialized() && (dead_zone[0] > dead_zone[1]))
            {
                return fgc4::utils::Warning("Upper edge of the dead_zone is below the lower edge.\n");
            }

            if (dead_zone.isInitialized() && (dead_zone[0] != dead_zone[1]))
            {
                m_dead_zone_defined = true;
            }
            else
            {
                m_dead_zone_defined = false;
            }

            if ((min.isInitialized() && max.isInitialized()) && (min >= max))
            {
                return fgc4::utils::Warning("Attempted to set the lower limit below the upper limit.\n");
            }

            return {};
        }

      private:
        int64_t m_head_integral{0};
        int64_t m_head_rms{0};

        T m_previous_value{};

        std::array<T, TimeWindowLength> integral_buffer{0};
        std::array<T, RMSBufferLength>  rms_buffer{0};

        bool m_dead_zone_defined{false};
    };
}