#include <cmath>

#include "clarkeTransform.h"

namespace vslib
{
    namespace
    {
        constexpr double SQRT_3_OVER_3 = sqrt(3) / 3.0;
        constexpr double TWO_OVER_3    = 2.0 / 3.0;
    }

    [[nodicard]] std::pair<double, double> ClarkeTransform::transform(double f_a, double f_b, double f_c) noexcept
    {
        const double two_pi_over_3 = 2.0 * M_PI / 3.0;

        const double alpha = TWO_OVER_3 * (f_a - 0.5 * (f_b + f_c));
        const double beta  = SQRT_3_OVER_3 * (f_b - f_c);

        return {alpha, beta};
    }

}   // namespace vslib