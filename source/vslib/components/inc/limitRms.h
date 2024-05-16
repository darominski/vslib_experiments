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
    class LimitRms : public Component
    {
      public:
        LimitRms(std::string_view name, Component* parent, double iteration_period = 5e-6)
            : Component("LimitRms", name, parent),
              rms_limit(*this, "rms_limit"),
              rms_time_constant(*this, "rms_time_constant", 1e-12),   // 1 ps limit
              m_iteration_period(iteration_period)   // TODO: replace by iteration period information sharing solution
        {
        }

        //! Checks the provided value against RMS limit and returns compliant value
        //!
        //! @param input Numerical input to be checked against set RMS limit
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        bool limit(double input) noexcept
        {
            if (std::isnan(input))
            {
                return false;
            }

            // calculation re-implemented from regLimRmsRT
            m_cumulative += (pow(input, 2) - m_cumulative) * m_filter_factor;

            if (m_cumulative > m_rms_limit2)
            {
                return false;
            }

            return true;
        }

        //! Checks the provided value against RMS limit and returns compliant value, non RT function
        //!
        //! @param input Numerical input to be checked against set RMS limit
        //! @return Optionally returns a Warning with relevant infraction information, nothing otherwise
        std::optional<fgc4::utils::Warning> limitNonRT(double input) noexcept
        {
            if (std::isnan(input))
            {
                return fgc4::utils::Warning(fmt::format("Value is a NaN.\n"));
            }

            // calculation re-implemented from regLimRmsRT
            m_cumulative += (pow(input, 2) - m_cumulative) * m_filter_factor;

            if (sqrt(m_cumulative) > rms_limit)
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
            m_cumulative = 0.0;
        }

        Parameter<double> rms_limit;
        Parameter<double> rms_time_constant;

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_filter_factor = m_iteration_period / (rms_time_constant.toValidate() + 0.5 * m_iteration_period);
            m_rms_limit2    = pow(rms_limit.toValidate(), 2);
            return {};
        }

      private:
        double m_iteration_period;
        double m_cumulative{0};
        double m_filter_factor;
        double m_rms_limit2;   // optimisation method to not recalculate limit to the power of 2
    };
}   // namespace vslib
