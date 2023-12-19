//! @file
//! @brief File with unit tests of BoxFirstOrderFilter class.
//! @author Dominik Arominski

#include <cmath>
#include <gtest/gtest.h>

#include "boxFirstOrderFilter.h"
#include "componentRegistry.h"

using namespace vslib;

class BoxFirstOrderFilterTest : public ::testing::Test
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

//! Checks that a BoxFirstOrderFilter object can be constructed
TEST_F(BoxFirstOrderFilterTest, FilterDefaultConstruction)
{
    BoxFirstOrderFilter filter("averaging_filter");
    EXPECT_EQ(filter.getName(), "averaging_filter");
}

//! Checks that a BoxFirstOrderFilter object can filter provided value
TEST_F(BoxFirstOrderFilterTest, FilterSingleValue)
{
    BoxFirstOrderFilter filter("filter", nullptr);
    double              value = 3.14159;
    EXPECT_NEAR(filter.filter(value), value / 2.0, 1e-6);
}

//! Checks that a BoxFirstOrderFilter object can filter a number of provided values
TEST_F(BoxFirstOrderFilterTest, FilterMultipleValues)
{
    constexpr int       buffer_length = 10;
    BoxFirstOrderFilter filter("filter", nullptr);
    std::vector<double> values(buffer_length);
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