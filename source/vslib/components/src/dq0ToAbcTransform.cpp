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

        const double a = d * sin(theta) + q * cos(theta) + zero;
        const double b = d * sin(theta_minus_two_thirds) + q * cos(theta_minus_two_thirds) + zero;
        const double c = d * sin(theta_plus_two_thirds) + q * cos(theta_plus_two_thirds) + zero;

        return {a, b, c};
    }

}   // namespace vslib