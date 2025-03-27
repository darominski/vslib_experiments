#pragma once

namespace user::constants
{
    // State machine constants

    //! Output voltage threshold, approximately zero V, with floating-point precision
    constexpr double v_out_threshold{1e-7};   //!< V
    //! Vdc lower threshold of floatings DCDC to go to blocking
    constexpr double v_dc_floatings_threshold{50};   //!< V
    //! Minimal value of Vdc below which the DC bus is considered discharged
    constexpr double v_dc_min = 50;   //!< V

}   // namespace user::constants