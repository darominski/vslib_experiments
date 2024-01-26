//! @file
//! @brief File with fixed-point type tests.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "fixedPointType.h"

using namespace vslib;

class FixedPointTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(FixedPointTest, CreateDefaultVariable)
{
    constexpr unsigned short franctional_bits = 24;

    FixedPoint<franctional_bits> fixed_point;
    EXPECT_EQ(FixedPoint<franctional_bits>::maximum_value, pow(2, 64 - franctional_bits - 1));
    EXPECT_EQ(fixed_point.toDouble(), 0.0);
}

TEST_F(FixedPointTest, CreateCustomVariable)
{
    constexpr unsigned short     franctional_bits = 15;
    double                       variable         = 3.14159;
    FixedPoint<franctional_bits> fixed_point      = variable;
    EXPECT_NEAR(fixed_point.toDouble(), variable, fixed_point.representation_precision);
    EXPECT_EQ(fixed_point.maximum_value, pow(2, 64 - franctional_bits - 1));
}

TEST_F(FixedPointTest, SumFixedPointVariables)
{
    constexpr unsigned short     franctional_bits   = 29;
    double                       variable1          = 3.14159;
    double                       variable2          = 2.7128;
    FixedPoint<franctional_bits> lhs                = variable1;
    FixedPoint<franctional_bits> rhs                = variable2;
    auto const                   result             = lhs + rhs;
    double const                 expected_precision = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(result.toDouble(), variable1 + variable2, expected_precision);
}

TEST_F(FixedPointTest, SubtractFixedPointVariables)
{
    constexpr unsigned short     franctional_bits   = 31;
    double                       variable1          = 3.14159;
    double                       variable2          = 2.7128;
    FixedPoint<franctional_bits> lhs                = variable1;
    FixedPoint<franctional_bits> rhs                = variable2;
    auto const                   result             = lhs - rhs;
    double const                 expected_precision = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(result.toDouble(), variable1 - variable2, expected_precision);
}

TEST_F(FixedPointTest, DivideFixedPointVariables)
{
    constexpr unsigned short     franctional_bits   = 19;
    double                       variable1          = 3.14159;
    double                       variable2          = 2.7128;
    FixedPoint<franctional_bits> lhs                = variable1;
    FixedPoint<franctional_bits> rhs                = variable2;
    auto const                   result             = lhs / rhs;
    double const                 expected_precision = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(result.toDouble(), variable1 / variable2, expected_precision);
}

TEST_F(FixedPointTest, MultiplyFixedPointVariables)
{
    constexpr unsigned short     franctional_bits   = 15;
    double                       variable1          = 3.14159;
    double                       variable2          = 2.7128;
    FixedPoint<franctional_bits> lhs                = variable1;
    FixedPoint<franctional_bits> rhs                = variable2;
    auto const                   result             = lhs * rhs;
    double const                 expected_precision = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(result.toDouble(), variable1 * variable2, expected_precision);
}

TEST_F(FixedPointTest, EqualSumFixedPointVariables)
{
    constexpr unsigned short     franctional_bits = 21;
    double                       variable1        = 3.14159;
    double                       variable2        = 2.7128;
    FixedPoint<franctional_bits> lhs              = variable1;
    FixedPoint<franctional_bits> rhs              = variable2;
    lhs                                           += rhs;
    double const expected_precision               = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(lhs.toDouble(), variable1 + variable2, expected_precision);
}

TEST_F(FixedPointTest, EqualSubtractFixedPointVariables)
{
    constexpr unsigned short     franctional_bits = 20;
    double                       variable1        = 3.14159;
    double                       variable2        = 2.7128;
    FixedPoint<franctional_bits> lhs              = variable1;
    FixedPoint<franctional_bits> rhs              = variable2;
    lhs                                           -= rhs;
    double const expected_precision               = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(lhs.toDouble(), variable1 - variable2, expected_precision);
}

TEST_F(FixedPointTest, EqualMultiplyFixedPointVariables)
{
    constexpr unsigned short     franctional_bits = 17;
    double                       variable1        = 3.14159;
    double                       variable2        = 2.7128;
    FixedPoint<franctional_bits> lhs              = variable1;
    FixedPoint<franctional_bits> rhs              = variable2;
    lhs                                           *= rhs;
    double const expected_precision               = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(lhs.toDouble(), variable1 * variable2, expected_precision);
}

TEST_F(FixedPointTest, EqualDivideFixedPointVariables)
{
    constexpr unsigned short     franctional_bits = 14;
    double                       variable1        = 3.14159;
    double                       variable2        = 2.7128;
    FixedPoint<franctional_bits> lhs              = variable1;
    FixedPoint<franctional_bits> rhs              = variable2;
    lhs                                           /= rhs;
    double const expected_precision               = pow(2, -franctional_bits + 1);
    EXPECT_NEAR(lhs.toDouble(), variable1 / variable2, expected_precision);
}

TEST_F(FixedPointTest, CompareFixedPointVariables)
{
    constexpr unsigned short     franctional_bits = 15;
    FixedPoint<franctional_bits> lhs              = 3.14159;
    FixedPoint<franctional_bits> rhs              = 2.7128;
    EXPECT_EQ(lhs > rhs, true);
    EXPECT_EQ(lhs < rhs, false);
    EXPECT_EQ(lhs >= rhs, true);
    EXPECT_EQ(lhs <= rhs, false);
    EXPECT_EQ(lhs == rhs, false);
    EXPECT_EQ(lhs != rhs, true);
}

TEST_F(FixedPointTest, CompareFixedPointVariablesCloseToMaximum)
{
    constexpr unsigned short franctional_bits  = 15;
    double const             close_to_maximum1 = pow(2, 64 - franctional_bits - 1) - 0.1;
    double const             close_to_maximum2 = pow(2, 64 - franctional_bits - 1) - 0.5;

    FixedPoint<franctional_bits> lhs(close_to_maximum1);
    FixedPoint<franctional_bits> rhs(close_to_maximum2);
    EXPECT_EQ(lhs > rhs, true);
    EXPECT_EQ(lhs < rhs, false);
    EXPECT_EQ(lhs >= rhs, true);
    EXPECT_EQ(lhs <= rhs, false);
    EXPECT_EQ(lhs == rhs, false);
    EXPECT_EQ(lhs != rhs, true);
}