//! @file
//! @brief Partial template specialization for RST controller of length 3 to be used as PID engine.
//! @author Dominik Arominski

#include "rstController.h"

namespace vslib
{
    //! Control method returning the next actuation
    //!
    //! @param input Current measurement value
    //! @param reference Current reference value
    //! @return Next actuation value
    template<>
    [[nodiscard]] double RSTController<3>::control(double input, double reference) noexcept
    {
        // This specialization allows to speed-up the calculation of the RST actuation by about 15%
        m_measurements[2] = m_measurements[1];
        m_measurements[1] = m_measurements[0];
        m_measurements[0] = input;

        m_references[2] = m_references[1];
        m_references[1] = m_references[0];
        m_references[0] = reference;

        m_actuations[2] = m_actuations[1];
        m_actuations[1] = m_actuations[0];
        m_actuations[0] = (m_t[0] * reference - m_r[0] * input + m_t[1] * m_references[1] - m_r[1] * m_measurements[1]
                           + m_t[2] * m_references[2] - m_r[2] * m_measurements[2]
                           - (m_s[1] * m_actuations[1] + m_s[2] * m_actuations[2]))
                          / m_s[0];
        return m_actuations[0];
    }

    //! Updates the most recent reference in the history, used in cases actuation goes over the limit
    //!
    //! @param updated_actuation Actuation that actually took place after clipping of the calculated actuation
    template<>
    void RSTController<3>::updateReference(double updated_actuation)
    {
        // based on logic of regRstCalcRefRT from CCLIBS libreg's regRst.c
        m_actuations[0] = updated_actuation;
        m_references[0] = m_s[0] * updated_actuation + m_r[0] * m_measurements[0] + m_s[1] * m_actuations[1]
                          + m_r[1] * m_measurements[1] - m_t[1] * m_references[1] + m_s[2] * m_actuations[2]
                          + m_r[2] * m_measurements[2] - m_t[2] * m_references[2];
    }

}   // namespace vslib