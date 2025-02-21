#include <cmath>

#include "dq0ToAlphaBetaTransform.hpp"

namespace vslib
{

    [[nodiscard]] std::tuple<double, double, double> Dq0ToAlphaBetaTransform::transform(
        const double d, const double q, const double zero, const double theta, const bool a_alignment
    ) noexcept
    {
        const double sin_theta = m_sin(theta);
        const double cos_theta = m_cos(theta);

        const double alpha = a_alignment ? d * cos_theta - q * sin_theta : d * sin_theta + q * cos_theta;
        const double beta  = a_alignment ? d * sin_theta + q * cos_theta : -d * cos_theta + q * sin_theta;

        return {alpha, beta, zero};
    }

}   // namespace vslib