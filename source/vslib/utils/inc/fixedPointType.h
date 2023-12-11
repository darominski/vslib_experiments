//! @file
//! @brief File containing definition of a base-10 fixed-point type.
//! @author Dominik Arominski

#pragma once

#include <compare>
#include <cstdint>

namespace vslib
{

    template<unsigned short MantissaBits>
    class FixedPoint
    {

        static_assert(MantissaBits <= 32, "Number of bits in the mantissa cannot be larger or equal to 32 bits!");

      public:
        FixedPoint()
            : m_value(0)
        {
        }

        FixedPoint(double floatValue)
            : m_value{static_cast<int64_t>(floatValue * static_cast<double>(int64_t(1) << MantissaBits))}
        {
        }

        [[nodiscard]] double toDouble() const
        {
            return static_cast<double>(m_value) / static_cast<double>(int64_t(1) << MantissaBits);
        }

        void operator+=(const FixedPoint& other)
        {
            m_value += other.value();
        }

        void operator-=(const FixedPoint& other)
        {
            m_value -= other.value();
        }

        void operator*=(const FixedPoint& other)
        {
            m_value *= other.value();
        }

        void operator/=(const FixedPoint& other)
        {
            m_value /= other.value();
        }

        auto operator<=>(const FixedPoint& other) const = default;

        FixedPoint operator+(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value + other.value();
            return result;
        }

        FixedPoint operator-(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value - other.value();
            return result;
        }

        FixedPoint operator*(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value * other.value() + (1 << (MantissaBits - 1))) >> MantissaBits;
            return result;
        }

        FixedPoint operator/(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value << MantissaBits) / other.value();
            return result;
        }

        const auto value() const noexcept
        {
            return m_value;
        }

      private:
        int64_t m_value;
    };

}   // namespace vslib