#include <cmath>
#include <numbers>

#include "alphaBetaToDq0Transform.hpp"

namespace vslib
{
    namespace
    {
        constexpr double one_over_3    = 1.0 / 3.0;
        constexpr double two_over_3    = 2.0 * one_over_3;
        constexpr double two_pi_over_3 = std::numbers::pi * two_over_3;
    }

    [[nodiscard]] std::tuple<double, double, double> AlphaBetaToDq0Transform::transform(
        const double f_alpha, const double f_beta, const double f_0, const double wt, const bool a_alignment
    ) noexcept
    {

        const double sin_theta = m_sin(wt);
        const double cos_theta = m_cos(wt);

        const double d
            = a_alignment ? f_alpha * cos_theta + f_beta * sin_theta : f_alpha * sin_theta - f_beta * cos_theta;
        const double q
            = a_alignment ? -f_alpha * sin_theta + f_beta * cos_theta : f_alpha * cos_theta + f_beta * sin_theta;

        return {d, q, f_0};
    }

}   // namespace vslib
