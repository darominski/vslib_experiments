#include <cmath>
#include <numbers>

#include "dq0ToAbcTransform.h"

namespace vslib
{
    [[nodiscard]] std::tuple<double, double, double> Dq0ToAbcTransform::transform(
        const double d, const double q, const double zero, const double wt, const double offset
    ) noexcept
    {
        const double theta = wt + offset;

        const auto [alpha, beta, zero_out] = dq0_2_alphabeta.transform(d, q, zero, theta, false);
        const auto [a, b, c]               = alphabeta_2_abc.transform(alpha, beta, zero_out);

        return {a, b, c};
    }

}   // namespace vslib