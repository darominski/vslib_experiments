#include <cmath>
#include <numbers>

#include "abcToDq0Transform.h"

namespace vslib
{
    namespace
    {
        constexpr double one_over_3    = 1.0 / 3.0;
        constexpr double two_over_3    = 2.0 * one_over_3;
        constexpr double two_pi_over_3 = std::numbers::pi * two_over_3;
    }

    [[nodiscard]] std::tuple<double, double, double> AbcToDq0Transform::transform(
        const double a, const double b, const double c, const double wt, const double offset
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

        const double d = two_over_3 * (a * sin_theta + b * sin_theta_minus_two_thirds + c * sin_theta_plus_two_thirds);
        const double q = two_over_3 * (a * cos_theta + b * cos_theta_minus_two_thirds + c * cos_theta_plus_two_thirds);
        const double zero = one_over_3 * (a + b + c);

        return {d, q, zero};
    }

}   // namespace vslib