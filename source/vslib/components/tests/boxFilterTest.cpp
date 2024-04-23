//! @file
//! @brief File with unit tests of BoxFilter class.
//! @author Dominik Arominski

#include <cmath>
#include <gtest/gtest.h>

#include "boxFilter.h"

using namespace vslib;

class BoxFilterTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks that a BoxFilter object can be constructed
TEST_F(BoxFilterTest, FilterDefaultConstruction)
{
    BoxFilter<3> filter("filter", nullptr);
    EXPECT_EQ(filter.getName(), "filter");
    EXPECT_EQ(filter.getMaxInputValue(), pow(2, std::ceil(log2(1e5))));
    EXPECT_EQ((BoxFilter<3, 1e5>::fractional_bits), 64 - 1 - std::ceil(log2(1e5)));
}

//! Checks that a BoxFilter object can be constructed with non-default mantissa template parameter
TEST_F(BoxFilterTest, FilterNonDefaultConstruction)
{
    constexpr double            maximal_value = 1e4;   // maximal value to be filtered
    BoxFilter<1, maximal_value> filter("filter", nullptr);
    EXPECT_EQ(filter.getName(), "filter");
    EXPECT_EQ(filter.getMaxInputValue(), pow(2, std::ceil(log2(maximal_value))));
}

//! Checks that a partial template specialization (1st order) object can filter provided value
TEST_F(BoxFilterTest, FirstOrderFilterSingleValue)
{
    BoxFilter<1> filter("filter", nullptr);
    double       value = 3.14159;
    EXPECT_NEAR(filter.filter(value), value / 2.0, 1e-6);
}

//! Checks that a partial template specialization (1st order) object can filter a number of provided values
TEST_F(BoxFilterTest, FirstOrderFilterMultipleValues)
{
    constexpr int       inputs_length = 10;
    BoxFilter<1>        filter("filter", nullptr);
    std::vector<double> values(inputs_length);
    std::iota(values.begin(), values.end(), 0);
    double previous_value = 0;
    int    counter        = 0;
    for (const auto& value : values)
    {
        const double average = (value + previous_value) / 2.0;
        previous_value       = value;
        EXPECT_NEAR(filter.filter(value), average, 1e-6);
    }
}

//! Checks that a partial template specialization (2nd order) object can filter provided value
TEST_F(BoxFilterTest, SecondOrderFilterSingleValue)
{
    BoxFilter<2> filter("filter", nullptr);
    double       value = 3.14159;
    EXPECT_NEAR(filter.filter(value), value / 3.0, 1e-6);
}

//! Checks that a partial template specialization (2nd order) object can filter a number of provided values
TEST_F(BoxFilterTest, SecondOrderFilterMultipleValues)
{
    constexpr int       inputs_length = 10;
    BoxFilter<2>        filter("filter", nullptr);
    std::vector<double> values(inputs_length);
    std::iota(values.begin(), values.end(), 0);
    double earlier_value  = 0;
    double previous_value = 0;
    int    counter        = 0;
    for (const auto& value : values)
    {
        const double average = (value + previous_value + earlier_value) / 3.0;
        earlier_value        = previous_value;
        previous_value       = value;
        EXPECT_NEAR(filter.filter(value), average, 1e-6);
    }
}

//! Checks that a BoxFilter object can filter provided value
TEST_F(BoxFilterTest, FilterSingleValue)
{
    constexpr int64_t       filter_order  = 9;
    constexpr int64_t       buffer_length = filter_order + 1;
    BoxFilter<filter_order> filter("filter", nullptr);
    double                  value = 3.14159;
    EXPECT_NEAR(filter.filter(value), value / buffer_length, 1e-5);
}

//! Checks that a BoxFilter object can filter a number of provided values
TEST_F(BoxFilterTest, FilterMultipleValues)
{
    constexpr int64_t       filter_order  = 9;
    constexpr int64_t       buffer_length = filter_order + 1;
    BoxFilter<filter_order> filter("filter", nullptr);
    std::vector<double>     values(filter_order);
    std::iota(values.begin(), values.end(), 0);
    double accumulator = 0;
    int    counter     = 0;
    for (const auto& value : values)
    {
        accumulator          += value;
        const double average = accumulator / buffer_length;
        EXPECT_NEAR(filter.filter(value), average, 1e-5);
    }
}

//! Checks that a BoxFilter filters correctly when buffer wraps around
TEST_F(BoxFilterTest, FilterValuesBufferWrapAround)
{
    constexpr int64_t       filter_order  = 5;
    constexpr int64_t       buffer_length = filter_order + 1;
    BoxFilter<filter_order> filter("filter", nullptr);
    std::vector<double>     values(10);
    std::iota(values.begin(), values.end(), 0);
    double accumulator = 0;
    int    counter     = 0;
    for (int index = 0; index < values.size(); index++)
    {
        double oldest_value = 0;
        if (index > filter_order)
        {
            oldest_value = values[index % buffer_length];
        }
        accumulator          += values[index] - oldest_value;
        const double average = accumulator / buffer_length;

        EXPECT_NEAR(filter.filter(values[index]), average, 1e-5);
    }
}