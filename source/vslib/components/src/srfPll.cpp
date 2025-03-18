#include "srfPll.hpp"

namespace vslib
{
    [[nodiscard]] std::pair<double, double>
    SRFPLL::synchroniseWithQ(const double f_a, const double f_b, const double f_c) noexcept
    {
        const auto [d, q, zero] = abc_2_dq0.transform(f_a, f_b, f_c, m_wt);

        // for consistency with Matlab, forward-Euler method is used instead of trapezoid
        // integration
        const auto wt = m_wt;
        // reference of the PI controller is always zero
        m_wt          += pi.control(0.0, -q) * pi.T + m_f_rated_2pi;
        // to avoid precision loss, the wt is limited to 0-2pi range
        m_wt          = fmod(m_wt, std::numbers::pi_v<double> * 2.0);

        return {wt + m_angle_offset, q};
    }

    [[nodiscard]] double SRFPLL::synchronise(const double f_a, const double f_b, const double f_c) noexcept
    {
        const auto [wt, _] = synchroniseWithQ(f_a, f_b, f_c);
        return wt;
    }

    void SRFPLL::reset() noexcept
    {
        m_wt = 0;
        pi.reset();
    }

    std::optional<fgc4::utils::Warning> SRFPLL::verifyParameters()
    {
        m_f_rated_2pi  = {2.0 * std::numbers::pi * f_rated.toValidate() * pi.T};
        m_angle_offset = angle_offset.toValidate();
        return {};
    }
}   // namespace vslib