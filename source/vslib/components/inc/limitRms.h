//! @file
//! @brief Defines the Component class for RMS limit protection.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    template<size_t RMSBufferLength = 16>
    class LimitRms : public Component
    {
      public:
        LimitRms(std::string_view name, Component* parent = nullptr)
            : Component("LimitRms", name, parent),
              rms_limit(*this, "rms_threshold"),
              rms_time_constant(*this, "rms_time_constant", 0, RMSBufferLength)
        {
        }

        //! Checks the RMS limit
        //!
        //! @param input Numerical input to be checked against set RMS limit
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limit(double input) noexcept
        {
            double const new_entry = abs(input) / sqrt(RMSBufferLength);
            m_cumulative           += new_entry - rms_buffer[m_head];

            rms_buffer[m_head] = new_entry;
            m_head++;
            if (m_head >= rms_time_constant)
            {
                m_head -= rms_time_constant;
            }

            if (m_cumulative > rms_limit)
            {
                return fgc4::utils::Warning(
                    fmt::format("Value: {} deviates too far from the RMS limit of {}.\n", input, rms_limit)
                );
            }

            return {};
        }

        //! Resets the component to the initial state of buffers and buffer pointers
        void reset() noexcept
        {
            m_head = 0;
            std::fill(std::begin(rms_buffer), std::end(rms_buffer), 0);
        }

        Parameter<double> rms_limit;
        Parameter<size_t> rms_time_constant;

      private:
        int64_t m_head{0};

        double m_cumulative{0};

        std::array<double, RMSBufferLength> rms_buffer{0};
    };
}   // namespace vslib