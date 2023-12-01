//! @file
//! @brief File with unit tests of BoxFilter class.
//! @author Dominik Arominski

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
    EXPECT_EQ(filter.getMaxInputValue(), 1e6);
}

//! Checks that a BoxFilter object can be constructed with non-default parameters
TEST_F(BoxFilterTest, FilterNonDefaultConstruction)
{
    BoxFilter<1> filter("filter", nullptr, 1e4);
    EXPECT_EQ(filter.getName(), "filter");
    EXPECT_EQ(filter.getMaxInputValue(), 1e4);
}

//! Checks that a BoxFilter object can filter provided value
TEST_F(BoxFilterTest, FilterSingleValue)
{
    BoxFilter<10> filter("filter", nullptr);
    double        value = 3.14159;
    EXPECT_NEAR(filter.filter(value), value, 1e-3);
}

//! Checks that a BoxFilter object can filter a number of provided values
TEST_F(BoxFilterTest, FilterMultipleValues)
{
    BoxFilter<10>       filter("filter", nullptr, 1e4);
    std::vector<double> values(10);
    std::iota(values.begin(), values.end(), 0);
    double accumulator = 0;
    int    counter     = 0;
    for (const auto& value : values)
    {
        accumulator += value;
        counter++;
        const double average = accumulator / counter;
        EXPECT_NEAR(filter.filter(value), average, 1e-3);
    }
}

//! Checks that a BoxFilter filters correctly when buffer wraps around
TEST_F(BoxFilterTest, FilterValuesBufferWrapAround)
{
    BoxFilter<5>        filter("filter", nullptr, 1e4);
    std::vector<double> values(10);
    std::iota(values.begin(), values.end(), 0);
    double accumulator = 0;
    int    counter     = 0;
    for (int index = 0; index < values.size(); index++)
    {
        counter             = index < 5 ? index + 1 : 5;   // never more than 5 elements in the filter
        double oldest_value = 0;
        if (index > 5)
        {
            oldest_value = values[index % 5];
        }
        accumulator          += values[index] - oldest_value;
        const double average = accumulator / counter;

        EXPECT_NEAR(filter.filter(values[index]), average, 1e-3);
    }
}