#include <cmath>
#include <numbers>

#include "instantaneousPowerThreePhase.hpp"

namespace vslib
{
    namespace
    {
        constexpr double inv_sqrt_3 = 1.0 / std::numbers::sqrt3;
    }

    [[nodiscard]] std::tuple<double, double> InstantaneousPowerThreePhase::calculate(
        const double v_a, const double v_b, const double v_c, const double i_a, const double i_b, const double i_c
    ) const noexcept
    {
        const double p_meas = (v_a * i_a + v_b * i_b + v_c * i_c);
        const double v_ab   = v_a - v_b;
        const double v_bc   = v_b - v_c;
        const double v_ca   = v_c - v_a;

        const double q_meas = (i_a * v_bc + i_b * v_ca + i_c * v_ab) * inv_sqrt_3;
        return {p_meas * p_gain, q_meas * q_gain};
    }
}   // namespace vslib