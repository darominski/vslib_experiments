#include <cmath>
#include <numbers>

#include "abcToAlphaBetaTransform.h"

namespace vslib
{
    namespace
    {
        constexpr double sqrt_3_over_3 = std::numbers::sqrt3 / 3.0;
        constexpr double one_over_3    = 1.0 / 3.0;
        constexpr double two_over_3    = 2.0 * one_over_3;
    }

    [[nodiscard]] std::tuple<double, double, double>
    AbcToAlphaBetaTransform::transform(const double f_a, const double f_b, const double f_c) noexcept
    {

        const double f_alpha = two_over_3 * (f_a - 0.5 * (f_b + f_c));
        const double f_beta  = sqrt_3_over_3 * (f_b - f_c);
        const double f_0     = one_over_3 * (f_a + f_b + f_c);

        return {f_alpha, f_beta, f_0};
    }

}   // namespace vslib