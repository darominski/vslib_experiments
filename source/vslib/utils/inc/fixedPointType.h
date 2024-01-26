//! @file
//! @brief File containing definition of a base-10 fixed-point type.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <compare>
#include <cstdint>

namespace vslib
{

    //! This class implements the Q notation to represent fixed-point numbers with the flexibility what the
    //! fractional precision shall be.
    //! Please note that no overflow nor underflow check is ever performed, just like with the standard C++ plain-old
    //! data types. The maximal value that can be stored without internal overflow is defined by the number of bits left
    //! for the exponent part and is accessible via maximumValue() public method.
    template<unsigned short FractionalBits>
    class FixedPoint
    {

      public:
        //! Default constructor, stored value initialized to zero.
        FixedPoint()
            : m_value(0)
        {
        }

        //! Constructor taking one double-precision floating-point value and converting it to Q notation
        FixedPoint(double floatValue)
            : m_value{static_cast<int64_t>(floatValue * m_fractional_shift)}
        {
        }

        //! Inverse conversion from the internal Q notation to double-precision floating point.
        [[nodiscard]] double toDouble() const
        {
            return static_cast<double>(m_value) / m_fractional_shift;
        }

        //! Overload to handle summing a FixedPoint object's value with the already-existing object
        void operator+=(const FixedPoint& other)
        {
            m_value += other.m_value;
        }

        //! Overload to handle subtracting a FixedPoint object's value from the already-existing object
        void operator-=(const FixedPoint& other)
        {
            m_value -= other.m_value;
        }

        //! Overload to handle multiplying a FixedPoint object's value with the already-existing object
        void operator*=(const FixedPoint& other)
        {
            m_value = (this->m_value * other.m_value + m_fractional_rounding) >> FractionalBits;
        }

        //! Overload to handle dividing the already-existing object's value by a FixedPoint object's value
        void operator/=(const FixedPoint& other)
        {
            m_value = (m_value << FractionalBits) / other.m_value;
        }

        //! Operator overload providing all 5 relationship checks, will only work where the number of fractional bits
        //! is consistent between this LHS and RHS objects.
        auto operator<=>(const FixedPoint& other) const = default;

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        FixedPoint operator+(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value + other.m_value;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        FixedPoint operator-(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value - other.m_value;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        FixedPoint operator*(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value * other.m_value + m_fractional_rounding) >> FractionalBits;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        FixedPoint operator/(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value << FractionalBits) / other.m_value;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        const auto value() const noexcept
        {
            return m_value;
        }

        inline static constexpr double maximum_value{
            pow(2, sizeof(int64_t) * 8 - FractionalBits - 1)};   // 8 bits per byte, -1 for sign

        inline static constexpr double representation_precision{pow(2, -FractionalBits)};

      private:
        int64_t                         m_value;   // value stored by the FixedPoint type
        inline static constexpr double  m_fractional_shift{static_cast<double>(int64_t(1) << FractionalBits)};
        inline static constexpr int64_t m_fractional_rounding{int64_t(1) << (FractionalBits - 1)};
    };

}   // namespace vslib