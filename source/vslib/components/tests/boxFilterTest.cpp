//! @file
//! @brief File with unit tests of BoxFilter class.
//! @author Dominik Arominski

#include <cmath>
#include <gtest/gtest.h>

#include "boxFilter.h"
#include "componentRegistry.h"

using namespace vslib;

class BoxFilterTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        ComponentRegistry& registry = ComponentRegistry::instance();
        registry.clearRegistry();
    }
};

//! Checks that a BoxFilter object can be constructed
TEST_F(BoxFilterTest, FilterDefaultConstruction)
{
    BoxFilter<1> filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
    EXPECT_EQ(filter.getMaxInputValue(), pow(2, std::ceil(log2(1e5))));
    EXPECT_EQ((BoxFilter<1, 1e5>::fractional_bits), 64 - 1 - std::ceil(log2(1e5)));
}

//! Checks that a BoxFilter object can be constructed with non-default mantissa template parameter
TEST_F(BoxFilterTest, FilterNonDefaultConstruction)
{
    constexpr double            maximal_value = 1e4;   // maximal value to be filtered
    BoxFilter<1, maximal_value> filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
    EXPECT_EQ(filter.getMaxInputValue(), pow(2, std::ceil(log2(maximal_value))));
}

//! Checks that a BoxFilter object can filter provided value
TEST_F(BoxFilterTest, FilterSingleValue)
{
    constexpr size_t         buffer_length = 10;
    BoxFilter<buffer_length> filter("filter", nullptr);
    double                   value = 3.14159;
    EXPECT_NEAR(filter.filter(value), value / buffer_length, 1e-5);
}

//! Checks that a BoxFilter object can filter a number of provided values
TEST_F(BoxFilterTest, FilterMultipleValues)
{
    constexpr size_t         buffer_length = 10;
    BoxFilter<buffer_length> filter("filter", nullptr);
    std::vector<double>      values(buffer_length);
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
    constexpr size_t         buffer_length = 5;
    BoxFilter<buffer_length> filter("filter", nullptr);
    std::vector<double>      values(10);
    std::iota(values.begin(), values.end(), 0);
    double accumulator = 0;
    int    counter     = 0;
    for (int index = 0; index < values.size(); index++)
    {
        double oldest_value = 0;
        if (index > buffer_length)
        {
            oldest_value = values[index % buffer_length];
        }
        accumulator          += values[index] - oldest_value;
        const double average = accumulator / buffer_length;

        EXPECT_NEAR(filter.filter(values[index]), average, 1e-5);
    }
}