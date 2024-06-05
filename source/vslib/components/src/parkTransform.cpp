#include "parkTransform.h"

namespace vslib
{
    std::pair<double, double> ParkTransform::transform(double a, double b, double c, double theta) noexcept
    {
        const double alpha = a;
        const double beta  = (a + 2 * b) / SQRT_3;

        const double cos_theta = m_cos(theta);
        const double sin_theta = m_sin(theta);

        const double d = alpha * cos_theta + beta * sin_theta;
        const double q = -alpha * sin_theta + beta * cos_theta;

        return {d, q};
    }

}   // namespace vslib