#include <cmath>

#include "parkTransform.h"

namespace vslib
{
    namespace
    {
        constexpr double TWO_OVER_3    = 2.0 / 3.0;
        constexpr double TWO_PI_OVER_3 = M_PI * TWO_OVER_3;
    }

    [[nodicard]] std::pair<double, double> ParkTransform::transform(double a, double b, double c, double theta) noexcept
    {
        const double cos_theta_minus_two_thirds = m_cos(theta - TWO_PI_OVER_3);
        const double cos_theta                  = m_cos(theta);
        const double cos_theta_plus_two_thirds  = m_cos(theta + TWO_PI_OVER_3);

        const double sin_theta_minus_two_thirds = m_sin(theta - TWO_PI_OVER_3);
        const double sin_theta                  = m_sin(theta);
        const double sin_theta_plus_two_thirds  = m_sin(theta + TWO_PI_OVER_3);

        const double d = TWO_OVER_3 * (a * sin_theta + b * sin_theta_minus_two_thirds + c * sin_theta_plus_two_thirds);
        const double q = TWO_OVER_3 * (a * cos_theta + b * cos_theta_minus_two_thirds + c * cos_theta_plus_two_thirds);

        return {d, q};
    }

}   // namespace vslib