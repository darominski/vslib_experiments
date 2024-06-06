#include "parkTransform.h"

namespace vslib
{
    std::pair<double, double> ParkTransform::transform(double a, double b, double c, double theta) noexcept
    {
        const double cos_theta = m_cos(theta);
        const double sin_theta = m_sin(theta);

        const double two_pi_over_3 = 2.0 * M_PI / 3.0;

        const double cos_theta_plus_two_thirds  = m_cos(theta + two_pi_over_3);
        const double cos_theta_minus_two_thirds = m_cos(theta - two_pi_over_3);

        const double sin_theta_plus_two_thirds  = m_sin(theta + two_pi_over_3);
        const double sin_theta_minus_two_thirds = m_sin(theta - two_pi_over_3);

        const double d = (2.0 / 3.0) * (a * sin_theta + b * sin_theta_minus_two_thirds + c * sin_theta_plus_two_thirds);
        const double q = (2.0 / 3.0) * (a * cos_theta + b * cos_theta_minus_two_thirds + c * cos_theta_plus_two_thirds);

        return {d, q};
    }

}   // namespace vslib