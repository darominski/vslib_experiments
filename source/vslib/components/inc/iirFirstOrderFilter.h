//! @file
//! @brief Defines class for a first-order infinite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "filter.h"
#include "parameter.h"

namespace vslib
{
    class IIRFirstOrderFilter : public Filter
    {
      public:
        //! Constructor of the first-order IIR filter component, initializing one Parameter: coefficients
        IIRFirstOrderFilter(std::string_view name, Component* parent = nullptr)
            : Filter("IIRFirstOrderFilter", name, parent),
              numerator(*this, "numerator_coefficients"),
              denominator(*this, "denominator_coefficients")
        {
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //! and previously filtered value's output.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            double const output
                = input * numerator[0] + m_previous_input * numerator[1] - m_previous_output * denominator[1];
            m_previous_input  = input;
            m_previous_output = output;
            return output;
        }

        Parameter<std::array<double, 2>> numerator;
        Parameter<std::array<double, 2>> denominator;

      private:
        double m_previous_input{0};
        double m_previous_output{0};
    };
}   // namespace vslib
