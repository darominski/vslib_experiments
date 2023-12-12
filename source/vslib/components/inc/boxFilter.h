//! @file
//! @brief Defines class for a finite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "filter.h"
#include "fixedPointType.h"

namespace vslib
{
    template<int64_t BufferLength, unsigned short FractionalBits = 24>
    class BoxFilter : public Filter
    {
      public:
        //! Constructor of the box filter component
        BoxFilter(std::string_view name, Component* parent = nullptr)
            : Filter("BoxFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(double input) override
        {
            auto const oldest_value = m_buffer[m_head];
            m_buffer[m_head]        = input;
            m_cumulative            += m_buffer[m_head] - oldest_value;
            m_head                  = (m_head + 1) % BufferLength;
            return (m_cumulative.toDouble()) / BufferLength;
        }

        [[nodiscard]] auto const getMaxInputValue() const noexcept
        {
            return FixedPoint<FractionalBits>::maximumValue();
        }

      private:
        std::array<FixedPoint<FractionalBits>, BufferLength> m_buffer{0};
        int64_t                                              m_head{0};
        FixedPoint<FractionalBits>                           m_cumulative{0};
    };
}   // namespace vslib
