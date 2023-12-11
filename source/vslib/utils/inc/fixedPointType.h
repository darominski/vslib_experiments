//! @file
//! @brief File containing definition of a base-10 fixed-point type.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <compare>
#include <cstdint>

namespace vslib
{

    template<unsigned short FractionalBits>
    class FixedPoint
    {

        static_assert(FractionalBits <= 32, "Number of the fractional bits cannot be larger or equal to 32 bits!");

      public:
        FixedPoint()
            : m_value(0)
        {
        }

        FixedPoint(double floatValue)
            : m_value{static_cast<int64_t>(floatValue * static_cast<double>(int64_t(1) << FractionalBits))}
        {
        }

        [[nodiscard]] double toDouble() const
        {
            return static_cast<double>(m_value) / static_cast<double>(int64_t(1) << FractionalBits);
        }

        void operator+=(const FixedPoint& other)
        {
            m_value += other.m_value;
        }

        void operator-=(const FixedPoint& other)
        {
            m_value -= other.m_value;
        }

        void operator*=(const FixedPoint& other)
        {
            m_value = (this->m_value * other.m_value + (int64_t(1) << (FractionalBits - 1))) >> FractionalBits;
        }

        void operator/=(const FixedPoint& other)
        {
            m_value = (m_value << FractionalBits) / other.m_value;
        }

        auto operator<=>(const FixedPoint& other) const = default;

        FixedPoint operator+(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value + other.m_value;
            return result;
        }

        FixedPoint operator-(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value - other.m_value;
            return result;
        }

        FixedPoint operator*(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value * other.m_value + (int64_t(1) << (FractionalBits - 1))) >> FractionalBits;
            return result;
        }

        FixedPoint operator/(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value << FractionalBits) / other.m_value;
            return result;
        }

        const auto value() const noexcept
        {
            return m_value;
        }

        static double maximumValue()
        {
            return m_max_value;
        }

      private:
        int64_t                        m_value;
        inline static constexpr double m_max_value{
            pow(2, sizeof(int64_t) * 8 - FractionalBits - 1)};   // 8 bits per byte, -1 for sign:
    };

}   // namespace vslib