#include <cmath>
#include <numbers>

#include "dq0ToAbcTransform.h"

namespace vslib
{
    namespace
    {
        constexpr double one_over_3    = 1.0 / 3.0;
        constexpr double two_over_3    = 2.0 * one_over_3;
        constexpr double two_pi_over_3 = std::numbers::pi * two_over_3;
    }

    [[nodiscard]] std::tuple<double, double, double> Dq0ToAbcTransform::transform(
        const double d, const double q, const double zero, const double wt, const double offset
    ) noexcept
    {
        const double theta                  = wt + offset;
        const double theta_minus_two_thirds = theta - two_pi_over_3;
        const double theta_plus_two_thirds  = theta + two_pi_over_3;

        const double sin_theta_minus_two_thirds = m_sin(theta_minus_two_thirds);
        const double sin_theta                  = m_sin(theta);
        const double sin_theta_plus_two_thirds  = m_sin(theta_plus_two_thirds);

        const double cos_theta_minus_two_thirds = m_cos(theta_minus_two_thirds);
        const double cos_theta                  = m_cos(theta);
        const double cos_theta_plus_two_thirds  = m_cos(theta_plus_two_thirds);

        const double a = d * sin_theta + q * cos_theta + zero;
        const double b = d * sin_theta_minus_two_thirds + q * cos_theta_minus_two_thirds + zero;
        const double c = d * sin_theta_plus_two_thirds + q * cos_theta_plus_two_thirds + zero;

        return {a, b, c};
    }

}   // namespace vslib