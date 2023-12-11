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
    constexpr unsigned short mantissa_length = 24;

    FixedPoint<mantissa_length> fixed_point;
    EXPECT_EQ(FixedPoint<mantissa_length>::maximumValue(), pow(2, 64 - mantissa_length - 1));
    EXPECT_EQ(fixed_point.toDouble(), 0.0);
}

TEST_F(FixedPointTest, CreateCustomVariable)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable        = 3.14159;
    FixedPoint<mantissa_length> fixed_point     = variable;
    EXPECT_NEAR(fixed_point.toDouble(), variable, 1e-3);
    EXPECT_EQ(fixed_point.maximumValue(), pow(2, 64 - mantissa_length - 1));
}

TEST_F(FixedPointTest, SumFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    auto const                  result          = lhs + rhs;
    EXPECT_NEAR(result.toDouble(), variable1 + variable2, 1e-3);
}

TEST_F(FixedPointTest, SubtractFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    auto const                  result          = lhs - rhs;
    EXPECT_NEAR(result.toDouble(), variable1 - variable2, 1e-3);
}

TEST_F(FixedPointTest, DivideFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    auto const                  result          = lhs / rhs;
    EXPECT_NEAR(result.toDouble(), variable1 / variable2, 1e-3);
}

TEST_F(FixedPointTest, MultiplyFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    auto const                  result          = lhs * rhs;
    EXPECT_NEAR(result.toDouble(), variable1 * variable2, 1e-3);
}

TEST_F(FixedPointTest, EqualSumFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    lhs                                         += rhs;
    EXPECT_NEAR(lhs.toDouble(), variable1 + variable2, 1e-3);
}

TEST_F(FixedPointTest, EqualSubtractFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    lhs                                         -= rhs;
    EXPECT_NEAR(lhs.toDouble(), variable1 - variable2, 1e-3);
}

TEST_F(FixedPointTest, EqualMultiplyFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    lhs                                         *= rhs;
    EXPECT_NEAR(lhs.toDouble(), variable1 * variable2, 1e-3);
}

TEST_F(FixedPointTest, EqualDivideFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    double                      variable1       = 3.14159;
    double                      variable2       = 2.7128;
    FixedPoint<mantissa_length> lhs             = variable1;
    FixedPoint<mantissa_length> rhs             = variable2;
    lhs                                         /= rhs;
    EXPECT_NEAR(lhs.toDouble(), variable1 / variable2, 1e-3);
}

TEST_F(FixedPointTest, CompareFixedPointVariables)
{
    constexpr unsigned short    mantissa_length = 15;
    FixedPoint<mantissa_length> lhs             = 3.14159;
    FixedPoint<mantissa_length> rhs             = 2.7128;
    EXPECT_EQ(lhs > rhs, true);
    EXPECT_EQ(lhs < rhs, false);
    EXPECT_EQ(lhs >= rhs, true);
    EXPECT_EQ(lhs <= rhs, false);
    EXPECT_EQ(lhs == rhs, false);
    EXPECT_EQ(lhs != rhs, true);
}