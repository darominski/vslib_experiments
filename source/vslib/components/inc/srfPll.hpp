//! @file
//! @brief Class definiting the Component interface of Synchronous Reference Frame phase-locked loop (SRF PLL).
//! @author Dominik Arominski

#pragma once

#include <string>
#include <tuple>

#include "abcToDq0Transform.hpp"
#include "component.hpp"
#include "parameter.hpp"
#include "pid.hpp"

namespace vslib
{
    class SRFPLL : public Component
    {
      public:
        //! Synchronous Reference Frame PLL constructor. Allows to set the name of the Component, set this controller as
        //! a child of a given parent Component, and specify an anti-windup function, which by default does not do
        //! anything.
        //!
        //! @param name Name identification of the PLL controller
        //! @param parent Parent of this controller
        SRFPLL(std::string_view name, Component& parent)
            : Component("SRFPLL", name, parent),
              f_rated(*this, "f_rated", 0.0),
              angle_offset(*this, "angle_offset"),
              abc_2_dq0("abc_2_dq0", *this, 50'000),   // 50'000 points needed to ensure 1e-6 relative precision of PLL
              pi("pi", *this)
        {
        }

        //! Computes one iteration of the the PLL synchronisation and returns wt and q.
        //!
        //! @param a A-phase component of the three-phase system
        //! @param b B-phase component of the three-phase system
        //! @param c C-phase component of the three-phase system
        //! @return Pair with balanced angle (omega t), that always fits in 0 to 2pi, d, and q
        [[nodiscard]] std::tuple<double, double, double>
        synchroniseWithDQ(const double f_a, const double f_b, const double f_c) noexcept;

        //! Computes one iteration of the the PLL synchronisation and returns wt.
        //!
        //! @param a A-phase component of the three-phase system
        //! @param b B-phase component of the three-phase system
        //! @param c C-phase component of the three-phase system
        //! @return Balanced angle (omega t), always fits in 0 to 2pi values
        [[nodiscard]] double synchronise(const double f_a, const double f_b, const double f_c) noexcept;

        //! Resets the controller to the initial state by zeroing the history.
        void reset() noexcept;

        // ************************************************************
        // Settable Parameters of the Component

        Parameter<double> f_rated;        //!< Frequency of the voltage source
        Parameter<double> angle_offset;   //!< Angle offset to be added to calculated wt

        // ************************************************************
        // Components owned by this Component

        AbcToDq0Transform abc_2_dq0;   //!< abc to dq0 transform part of the SRF PLL
        PID               pi;          //!< PI controller part of the SRF PLL

        // ************************************************************

        //! Update parameters method, called after any Parameter of this Component is modified.
        //!
        //! @return Optionally returns a Warning with pertinent information if verification was unsuccessful, nothing
        //! otherwise
        std::optional<fgc4::utils::Warning> verifyParameters() override;

      private:
        double m_wt{0.0};             // Returned wt value of the PLL
        double m_angle_offset{0.0};   // Angular offset of the PLL output
        double m_f_rated_2pi{0.0};    // 2 pi * f_rated * (Euler step size)
    };
}   // namespace vslib
