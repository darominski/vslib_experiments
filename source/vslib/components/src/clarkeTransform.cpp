#include <cmath>

#include "clarkeTransform.h"

namespace vslib
{
    namespace
    {
        constexpr double sqrt_3_over_3 = sqrt(3.0) / 3.0;
        constexpr double two_over_3    = 2.0 / 3.0;
    }

    [[nodiscard]] std::pair<double, double> ClarkeTransform::transform(double f_a, double f_b, double f_c) noexcept
    {

        const double alpha = two_over_3 * (f_a - 0.5 * (f_b + f_c));
        const double beta  = sqrt_3_over_3 * (f_b - f_c);

        return {alpha, beta};
    }

}   // namespace vslib