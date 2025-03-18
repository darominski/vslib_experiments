//! @file
//! @brief Tests for simple statistics methods.
//! @author Dominik Arominski

#include <array>
#include <cmath>
#include <gtest/gtest.h>
#include <numeric>

#include "statistics.hpp"

using namespace vslib::utils;

class StatisticsTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StatisticsTest, CalculateTrivialAverage)
{
    constexpr unsigned short       array_size  = 10;
    double const                   input_value = 3.14159;
    std::array<double, array_size> numbers{input_value};

    ASSERT_EQ(calculateAverage(numbers), input_value / array_size);
}

TEST_F(StatisticsTest, CalculateAverage)
{
    constexpr unsigned short       array_size = 24;
    std::array<double, array_size> numbers;
    double const                   starting_value = 1.5;
    std::iota(numbers.begin(), numbers.end(), starting_value);

    double expected_average = 0.5 * (starting_value * 2 + 23);
    ASSERT_EQ(calculateAverage(numbers), expected_average);
}

TEST_F(StatisticsTest, CalculateTrivialStdDeviation)
{
    constexpr unsigned short       array_size  = 10;
    double const                   input_value = 3.14159;
    std::array<double, array_size> numbers{input_value};

    double const average        = input_value / array_size;
    double const expected_value = sqrt((pow(average - input_value, 2) + 9 * pow(average, 2)) / (array_size - 1));
    ASSERT_NEAR(calculateStandardDeviation(numbers, average), expected_value, 1e-5);
}

TEST_F(StatisticsTest, CalculateStdDeviation)
{
    constexpr unsigned short       array_size = 24;
    std::array<double, array_size> numbers;
    double const                   starting_value = 1.5;
    std::iota(numbers.begin(), numbers.end(), starting_value);

    double const average     = calculateAverage(numbers);
    double       squared_sum = 0;
    for (const auto& value : numbers)
    {
        squared_sum += pow(value - average, 2);
    }
    double const expected_value = sqrt(squared_sum / (array_size - 1));

    ASSERT_NEAR(calculateStandardDeviation(numbers, average), expected_value, 1e-5);
}
