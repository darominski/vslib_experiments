//! @file
//! @brief Unit tests of generateFunction function.
//! @author Dominik Arominski

#include <cmath>
#include <gtest/gtest.h>
#include <string>

#include "functionGenerator.hpp"

using namespace fgc4::utils;

class GenerateFunction : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks that linear function over the expected range can be created
TEST(GenerateFunction, LinearIntFunction)
{
    auto fun = [](const auto x)
    {
        return 2 * x;
    };
    const int min      = -10;
    const int max      = 10;
    const int n_points = 20;
    auto      function = generateFunction<int, int>(fun, min, max, n_points);

    EXPECT_EQ(function.size(), n_points + 1);
    EXPECT_TRUE(all_of(
        function.cbegin(), function.cend(),
        [&](const auto& point)
        {
            return (point.second == fun(point.first));
        }
    ));
}

//! Checks that linear function over the expected range can be created
TEST(GenerateFunction, LinearDoubleFunction)
{
    auto fun = [](const auto x)
    {
        return 2 * x;
    };
    const double min      = -15;
    const double max      = 321;
    const size_t n_points = 20000;
    auto         function = generateFunction<double, double>(fun, min, max, n_points);

    EXPECT_EQ(function.size(), n_points + 1);
    EXPECT_TRUE(all_of(
        function.cbegin(), function.cend(),
        [&](const auto& point)
        {
            return (point.second == fun(point.first));
        }
    ));
}

//! Checks that assert is raised when max < min
TEST(GenerateFunction, MinOverMax)
{
    auto fun = [](const auto x)
    {
        return 2 * x;
    };
    const double min      = -15;
    const double max      = min - 5;
    const size_t n_points = 20000;
    ASSERT_DEATH((generateFunction<double, double>(fun, min, max, n_points)), "Assertion `max > min' failed.");
}

//! Checks that trigonometric sin function over the expected range can be created
TEST(GenerateFunction, SinFunction)
{
    const double min      = 0.0;
    const double max      = 2.0 * M_PI;
    const size_t n_points = 1000;
    auto         function = generateFunction<double, double>(sin, min, max, n_points);

    EXPECT_EQ(function.size(), n_points + 1);
    EXPECT_TRUE(all_of(
        function.cbegin(), function.cend(),
        [&](const auto& point)
        {
            return (point.second == sin(point.first));
        }
    ));
}

//! Checks that trigonometric cos function over the expected range can be created
TEST(GenerateFunction, CosFunction)
{
    const double min      = 0.0;
    const double max      = 2.0 * M_PI;
    const size_t n_points = 1000;
    auto         function = generateFunction<double, double>(cos, min, max, n_points);

    EXPECT_EQ(function.size(), n_points + 1);
    EXPECT_TRUE(all_of(
        function.cbegin(), function.cend(),
        [&](const auto& point)
        {
            return (point.second == cos(point.first));
        }
    ));
}