#include <cmath>
#include <numbers>

#include "alphaBetaToAbcTransform.h"

namespace vslib
{
    namespace
    {
        constexpr static double sqrt_3_over_2 = 0.5 * std::numbers::sqrt3;
    }

    [[nodiscard]] std::tuple<double, double, double>
    AlphaBetaToAbcTransform::transform(const double f_alpha, const double f_beta, const double f_zero) noexcept
    {

        const double f_a = f_alpha + f_zero;
        const double f_b = -0.5 * f_alpha + sqrt_3_over_2 * f_beta + f_zero;
        const double f_c = -0.5 * f_alpha - sqrt_3_over_2 * f_beta + f_zero;

        return {f_a, f_b, f_c};
    }

}   // namespace vslib