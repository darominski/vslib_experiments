#include <cmath>
#include <numbers>

#include "parkTransform.h"

namespace vslib
{
    namespace
    {
        constexpr float one_over_3    = 1.0 / 3.0;
        constexpr float two_over_3    = 2.0 * one_over_3;
        constexpr float two_pi_over_3 = std::numbers::pi * two_over_3;
    }

    [[nodiscard]] std::tuple<float, float, float> ParkTransform::transform(
        const float a, const float b, const float c, const float theta, const float offset
    ) noexcept
    {
        const float cos_theta_minus_two_thirds = m_cos(theta - two_pi_over_3 + offset);
        const float cos_theta                  = m_cos(theta + offset);
        const float cos_theta_plus_two_thirds  = m_cos(theta + two_pi_over_3 + offset);

        const float sin_theta_minus_two_thirds = m_sin(theta - two_pi_over_3 + offset);
        const float sin_theta                  = m_sin(theta + offset);
        const float sin_theta_plus_two_thirds  = m_sin(theta + two_pi_over_3 + offset);

        const float d = two_over_3 * (a * sin_theta + b * sin_theta_minus_two_thirds + c * sin_theta_plus_two_thirds);
        const float q = two_over_3 * (a * cos_theta + b * cos_theta_minus_two_thirds + c * cos_theta_plus_two_thirds);
        const float zero = one_over_3 * (a + b + c);

        return {d, q, zero};
    }

}   // namespace vslib