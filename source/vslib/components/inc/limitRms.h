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
        //! Constructor for the LimitRms Component, initializes rms_limit, and rms_time_constant Parameters.
        //!
        //! @param name Name of this Limit Component
        //! @param parent Parent of this Limit Component
        //! @param iteration_period Iteration period at which this Limit Component is called
        LimitRms(std::string_view name, Component& parent, const double iteration_period = 5e-6)
            : Component("LimitRms", name, parent),
              rms_limit(*this, "rms_limit"),
              rms_time_constant(*this, "rms_time_constant", 1e-12),   // 1 ps limit
              m_iteration_period(iteration_period)   // TODO: replace by iteration period information sharing solution
        {
        }

        //! Checks the provided value against RMS limit and returns compliant value
        //!
        //! @param input Numerical input to be checked against set RMS limit
        //! @return Returns true if the provided input does not violate the limit, false otherwise
        [[nodiscard]] bool limit(const double input) noexcept
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

        //! Resets this Limit Component to the initial state of buffers and buffer pointers.
        void reset() noexcept
        {
            m_cumulative = 0.0;
        }

        Parameter<double> rms_limit;           //!< Maximal value of root-mean square
        Parameter<double> rms_time_constant;   //!< Time constant to calculate filter factor

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_filter_factor = m_iteration_period / (rms_time_constant.toValidate() + 0.5 * m_iteration_period);
            m_rms_limit2    = pow(rms_limit.toValidate(), 2);
            return {};
        }

      private:
        double m_iteration_period;   //!< Iteration period for this Limit
        double m_cumulative{0};      //!< Cumulative of the squared inputs
        double m_filter_factor;      //!< Convenience factor to avoid re-calculation each call to limit
        double m_rms_limit2;         //!< optimisation method to not recalculate limit to the power of 2
    };
}   // namespace vslib
