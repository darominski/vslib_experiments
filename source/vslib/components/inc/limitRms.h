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
    template<typename T, size_t RMSBufferLength = 16>
    class Limit : public Component
    {
      public:
        Limit(std::string_view name, Component* parent = nullptr)
            : Component("Limit", name, parent),
              rms(*this, "rms_threshold"),
              rms_time_constant(*this, "rms_time_constant", 0, RMSBufferLength)
        {
        }

        //! Checks the RMS limit
        //!
        //! @param input Numerical input to be checked against set RMS limit
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limit(T input) noexcept
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

        //! Resets the component to the initial state of buffers and buffer pointers
        void reset() noexcept
        {
            m_head_rms = 0;
            std::fill(std::begin(rms_buffer), std::end(rms_buffer), 0);
        }

        Parameter<double> rms;
        Parameter<size_t> rms_time_constant;

      private:
        int64_t m_head_rms{0};

        std::array<T, RMSBufferLength> rms_buffer{0};
    };
}   // namespace vslib