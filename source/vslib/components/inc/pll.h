//! @file
//! @brief Class definiting the Component interface of Phase-locked loop (PLL).
//! @author Dominik Arominski

#pragma once

#include <string>

#include "abcToDq0Transform.h"
#include "component.h"
#include "parameter.h"
#include "pid.h"

namespace vslib
{
    class PLL : public Component
    {

      public:
        //! PLL constructor. Allows to set the name of the Component, set this controller as a child of a
        //! given parent Component, and specify an anti-windup function, which by default does not do anything.
        //!
        //! @param name Name identification of the PLL controller
        //! @param parent Parent of this controller
        PLL(std::string_view name, IComponent& parent)
            : Component("PLL", name, parent),
              f_rated(*this, "f_rated", 0.0),
              angle_offset(*this, "angle_offset"),
              abc_2_dq0("abc_2_dq0", *this),
              pi("pi", *this),
              integrator("i", *this)
        {
        }

        //! Computes one iteration of the balancing of the PLL.
        //!
        //! @param a A-phase component of the three-phase system
        //! @param b B-phase component of the three-phase system
        //! @param c C-phase component of the three-phase system
        //! @return Balanced angle (omega t)
        [[nodiscard]] double balance(const double a, const double b, const double c) noexcept
        {
            const auto [d, q, zero] = abc_2_dq0.transform(a, b, c, m_wt);

            // reference of the PI controller is always zero
            const double pi_out = pi.control(-q, 0.0);
            m_wt                = integrator.control(0.0, pi_out + m_f_rated);

            return (m_wt + m_angle_offset);
        }

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept
        {
            m_wt = 0;
            pi.reset();
        }

        // ************************************************************
        // Settable Parameters of the Component

        Parameter<double> f_rated;        //!< Frequency of the voltage source
        Parameter<double> angle_offset;   //!< Angle offset to be added to calculated wt

        // ************************************************************
        // Components owned by this Component

        AbcToDq0Transform abc_2_dq0;    //!< abc to dq0 transform part of the PLL
        PID               pi;           //!< PI controller part of the PLL
        PID               integrator;   //!< I controller part of the PLL, accumulates wt with 2pi*f

        // ************************************************************

        //! Update parameters method, called after any Parameter of this Component is modified.
        //!
        //! @return Optionally returns a Warning with pertinent information if verification was unsuccessful, nothing
        //! otherwise
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_f_rated      = {2.0 * std::numbers::pi * f_rated.toValidate()};
            m_angle_offset = angle_offset.toValidate();
            return {};
        }

      private:
        double m_wt{0.0};             // Returned wt value of the PLL
        double m_angle_offset{0.0};   // Fixed for the moment
        double m_f_rated{0.0};        // 2 pi 50 Hz, fixed for the moment
    };
}   // namespace vslib
