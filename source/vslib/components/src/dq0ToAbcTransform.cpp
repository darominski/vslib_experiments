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

        constexpr bool a_axis_alignment
            = false;   //! alignment between A-axis and d-axis, false: A axis is 90 degrees behind
        const auto [alpha, beta, zero_out] = dq0_to_alphabeta.transform(d, q, zero, theta, a_axis_alignment);
        const auto [a, b, c]               = alphabeta_to_abc.transform(alpha, beta, zero_out);

        return {a, b, c};
    }

}   // namespace vslib