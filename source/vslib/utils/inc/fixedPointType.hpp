//! @file
//! @brief File containing definition of a base-10 fixed-point type.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <compare>
#include <cstdint>

#include "typeTraits.hpp"

namespace vslib
{

    //! This class implements the Q notation to represent fixed-point numbers with the flexibility what the
    //! fractional precision shall be.
    //! Please note that no overflow nor underflow check is ever performed, just like with the standard C++ plain-old
    //! data types. The maximal value that can be stored without internal overflow is defined by the number of bits left
    //! for the exponent part and is accessible via maximumValue() public method.
    template<int16_t fractional_bits, fgc4::utils::Integral T = int64_t>
    class FixedPoint
    {
      public:
        //! Default constructor, stored value initialized to zero.
        FixedPoint()
            : m_value(0)
        {
        }

        //! Constructor taking one double-precision floating-point value and converting it to Q notation
        //!
        //! @param float_value Floating-point value to be represented
        FixedPoint(const double float_value)
            : m_value{static_cast<T>(float_value * m_fractional_shift)}
        {
        }

        //! Constructor taking one single-precision floating-point value and converting it to Q notation
        //!
        //! @param float_value Floating-point value to be represented
        FixedPoint(const float float_value)
            : m_value{static_cast<T>(float_value * m_float_fractional_shift)}
        {
        }

        //! Constructor taking one double-precision floating-point value and converting it to Q notation
        //!
        //! @param value Floating-point value to be represented
        FixedPoint(const int value)
            : m_value{static_cast<T>(value)}
        {
        }

        //! Inverse conversion from the internal Q notation to double-precision floating point.
        [[nodiscard]] double toDouble() const
        {
            return static_cast<double>(m_value) / m_fractional_shift;
        }

        //! Inverse conversion from the internal Q notation to double-precision floating point.
        [[nodiscard]] float toFloat() const
        {
            return static_cast<float>(m_value) / m_float_fractional_shift;
        }

        //! Overload to handle summing a FixedPoint object's value with the already-existing object
        //!
        //! @param other Fixed-point value to be added
        void operator+=(const FixedPoint& other)
        {
            m_value += other.m_value;
        }

        //! Overload to handle subtracting a FixedPoint object's value from the already-existing object
        //!
        //! @param other Fixed-point value to be subtracted
        void operator-=(const FixedPoint& other)
        {
            m_value -= other.m_value;
        }

        //! Overload to handle multiplying a FixedPoint object's value with the already-existing object
        //!
        //! @param other Fixed-point value to be the multiplicand
        void operator*=(const FixedPoint& other)
        {
            m_value = (this->m_value * other.m_value + m_fractional_rounding) >> fractional_bits;
        }

        //! Overload to handle dividing the already-existing object's value by a FixedPoint object's value
        //!
        //! @param Fixed-point value to be the divisor
        void operator/=(const FixedPoint& other)
        {
            m_value = (m_value << fractional_bits) / other.m_value;
        }

        //! Operator overload providing all 5 relationship checks, will only work where the number of fractional bits
        //! is consistent between this LHS and RHS objects.
        auto operator<=>(const FixedPoint& other) const = default;

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        //!
        //! @param other Fixed-point value to be the addend
        FixedPoint operator+(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value + other.m_value;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        //!
        //! @param other Fixed-point value to be the subtrahend
        FixedPoint operator-(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = this->m_value - other.m_value;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        //!
        //! @param other Fixed-point value of the multiplicand
        FixedPoint operator*(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value * other.m_value + m_fractional_rounding) >> fractional_bits;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        //!
        //! @param other Fixed-point value to be the divisor
        FixedPoint operator/(const FixedPoint& other) const
        {
            FixedPoint result;
            result.m_value = (this->m_value << fractional_bits) / other.m_value;
            return result;
        }

        //! Overload to handle summing two FixedPoint objects where a new object needs to be created.
        //!
        //! @return Value of the fixed point object
        const auto value() const noexcept
        {
            return m_value;
        }

        //!< Maximum value that can be stored by the fixed-point object
        inline static constexpr double maximum_value{
            pow(2, sizeof(T) * 8 - fractional_bits - 1)};   // 8 bits per byte, -1 for sign

        //!< Representation precision of the fixed-point object
        inline static constexpr double representation_precision{pow(2, -fractional_bits)};

      private:
        T m_value;   //!< value stored by the FixedPoint type
        //!< Helper method holding the fractional bit shift
        inline static constexpr double m_fractional_shift{static_cast<double>(T(1) << fractional_bits)};
        inline static constexpr float  m_float_fractional_shift{static_cast<float>(T(1) << fractional_bits)};
        //!< Helper method holding the fractional rounding
        inline static constexpr T m_fractional_rounding{T(1) << (fractional_bits - 1)};
    };

}   // namespace vslib