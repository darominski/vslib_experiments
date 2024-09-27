#include <cmath>
#include <numbers>

#include "abcToDq0Transform.h"

namespace vslib
{
    [[nodiscard]] std::tuple<double, double, double> AbcToDq0Transform::transform(
        const double a, const double b, const double c, const double wt, const double offset
    ) noexcept
    {
        const double theta = wt + offset;
        // this two-step calculation was found to be almost 50% more performant than a direct calculation
        // from abc to dq0, due to fewer lookups to the sine and cosine tables.

        const auto [alpha, beta, zero_1] = abc_2_alphabeta.transform(a, b, c);
        const auto [d, q, zero]          = alphabeta_2_dq0.transform(alpha, beta, zero_1, theta, false);

        return {d, q, zero};
    }

}   // namespace vslib