//! @file
//! @brief Defines class for a finite-impulse filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <string>

#include "filter.h"
#include "fixedPointType.h"

namespace vslib
{
    template<uint64_t BufferLength, double maximalFilteredValue = 1e5>
    class BoxFilter : public Filter
    {

      public:
        //! 8 is the number of bits per byte, -1 is for the sign
        constexpr uint64_t static fractional_bits = sizeof(int64_t) * 8 - 1 - std::ceil(log2(maximalFilteredValue));

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
            return FixedPoint<fractional_bits>::maximumValue();
        }

      private:
        std::array<FixedPoint<fractional_bits>, BufferLength> m_buffer{0};
        uint64_t                                              m_head{0};
        FixedPoint<fractional_bits>                           m_cumulative{0};
    };
}   // namespace vslib
