#include <cmath>
#include <numbers>

#include "clarkeParkTransform.h"

namespace vslib
{
    namespace
    {
        constexpr double one_over_3    = 1.0 / 3.0;
        constexpr double two_over_3    = 2.0 * one_over_3;
        constexpr double two_pi_over_3 = std::numbers::pi * two_over_3;
    }

    [[nodiscard]] std::tuple<double, double, double>
    ClarkeParkTransform::transform(double f_alpha, double f_beta, double f_0, double theta, bool a_alignment) noexcept
    {

        const double sin_theta = m_sin(theta);
        const double cos_theta = m_cos(theta);

        double d;
        double q;
        double zero;

        if (a_alignment)   // A-axis alignment
        {
            d    = f_alpha * cos_theta + f_beta * sin_theta;
            q    = -f_alpha * sin_theta + f_beta * cos_theta;
            zero = f_0;
        }
        else   // 90 degrees behind A axis
        {
            const double u_a = f_alpha + f_0;
            const double u_b = 0.5 * (-f_alpha + std::numbers::sqrt3 * f_beta) + f_0;
            const double u_c = -0.5 * (f_alpha + std::numbers::sqrt3 * f_beta) + f_0;

            const double sin_theta_minus_two_thirds = m_sin(theta - two_pi_over_3);
            const double sin_theta_plus_two_thirds  = m_sin(theta + two_pi_over_3);

            const double cos_theta_minus_two_thirds = m_cos(theta - two_pi_over_3);
            const double cos_theta_plus_two_thirds  = m_cos(theta + two_pi_over_3);

            d    = two_over_3 * (u_a * sin_theta + u_b * sin_theta_minus_two_thirds + u_c * sin_theta_plus_two_thirds);
            q    = two_over_3 * (u_a * cos_theta + u_b * cos_theta_minus_two_thirds + u_c * cos_theta_plus_two_thirds);
            zero = one_over_3 * (u_a + u_b + u_c);
        }

        return {d, q, zero};
    }

}   // namespace vslib