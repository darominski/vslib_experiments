#include "iirFilter.h"

namespace vslib
{
    // ************************************************************
    // Partial template specialization for the first-order filter. Specialization of functions avoids repetition
    // of the entire class structure.
    //
    // Benchmarking showed 19% gain for the first order, and only 4% for the 2nd order. Therefore, only
    // the first order is specialized.

    template<>
    [[nodicard]] double IIRFilter<1>::filter(const double input)
    {
        auto const previous_input  = m_inputs_buffer[0];
        auto const previous_output = m_outputs_buffer[0];

        double const output
            = input * m_numerator[0] + previous_input * m_numerator[1] - previous_output * m_denominator[1];

        // update input and output buffers
        m_inputs_buffer[0]  = input;
        m_outputs_buffer[0] = output;

        return output;
    }
}   // namespace vslib