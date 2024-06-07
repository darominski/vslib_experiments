#include "firFilter.h"

namespace vslib
{
    // ************************************************************
    // Partial template specialization for low-order filters. Specialization of functions avoids repetition
    // of the entire class structure.
    //
    // Benchmarking showed 44% gain for the first order, and 72% for the 2nd order.

    template<>
    [[nodicard]] double FIRFilter<1>::filter(const double input)
    {
        auto const   previous_input = m_buffer[0];
        double const output         = input * m_coefficients[0] + previous_input * m_coefficients[1];
        m_buffer[0]                 = input;   // update input buffer

        return output;
    }

    template<>
    [[nodicard]] double FIRFilter<2>::filter(const double input)
    {
        auto const earlier_input  = m_buffer[0];
        auto const previous_input = m_buffer[1];

        double const output
            = input * m_coefficients[0] + previous_input * m_coefficients[1] + earlier_input * m_coefficients[2];

        // update input buffer
        m_buffer[0] = m_buffer[1];
        m_buffer[1] = input;
        return output;
    }
}   // namespace vslib