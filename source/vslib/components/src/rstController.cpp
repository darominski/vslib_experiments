//! @file
//! @brief Partial template specialization for RST controller of length 3 to be used as PID engine.
//! @author Dominik Arominski

#include "rstController.h"

namespace vslib
{
    //! Control method returning the next actuation
    //!
    //! @param process_value Current easurment value
    //! @param reference Current reference value
    //! @return Next actuation value
    template<>
    double RSTController<3>::control(double input, double reference) noexcept
    {
        // This partial template specialization allows to speed-up the calculation of the RST actuation by about 15%
        const double actuation = (m_t[0] * reference - m_r[0] * input + m_t[1] * m_references[1]
                                  - m_r[1] * m_measurements[1] + m_t[2] * m_references[2] - m_r[2] * m_measurements[2]
                                  - (m_s[1] * m_actuations[1] + m_s[2] * m_actuations[2]))
                                 / m_s[0];

        m_actuations[2] = m_actuations[1];
        m_actuations[1] = actuation;

        m_measurements[2] = m_measurements[1];
        m_measurements[1] = input;

        m_references[2] = m_references[1];
        m_references[1] = reference;

        return actuation;
    }

}   // namespace vslib