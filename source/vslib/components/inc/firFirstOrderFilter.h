//! @file
//! @brief Defines class for a first order finite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "filter.h"
#include "parameter.h"

namespace vslib
{
    class FIRFirstOrderFilter : public Filter
    {
      public:
        //! Constructor of the FIR filter component, initializing one Parameter: coefficients
        FIRFirstOrderFilter(std::string_view name, Component* parent = nullptr)
            : Filter("FIRFirstOrderFilter", name, parent),
              coefficients(*this, "coefficients")
        {
        }

        //! Filters the provided input by convolving coefficients and the input, including previous inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(const double input) override
        {
            double const output = input * coefficients[0] + m_previous_input * coefficients[1];
            m_previous_input    = input;
            return output;
        }

        Parameter<std::array<double, 2>> coefficients;

      private:
        double m_previous_input{0.0};
    };
}   // namespace vslib
