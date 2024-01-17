//! @file
//! @brief File with unit tests of BoxSecondOrderFilter class.
//! @author Dominik Arominski

#include <cmath>
#include <gtest/gtest.h>

#include "boxSecondOrderFilter.h"
#include "componentRegistry.h"

using namespace vslib;

class BoxSecondOrderFilterTest : public ::testing::Test
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

//! Checks that a BoxSecondOrderFilter object can be constructed
TEST_F(BoxSecondOrderFilterTest, FilterDefaultConstruction)
{
    BoxSecondOrderFilter filter("averaging_filter");
    EXPECT_EQ(filter.getName(), "averaging_filter");
}

//! Checks that a BoxSecondOrderFilter object can filter provided value
TEST_F(BoxSecondOrderFilterTest, FilterSingleValue)
{
    BoxSecondOrderFilter filter("filter", nullptr);
    double               value = 3.14159;
    EXPECT_NEAR(filter.filter(value), value / 3.0, 1e-6);
}

//! Checks that a BoxSecondOrderFilter object can filter a number of provided values
TEST_F(BoxSecondOrderFilterTest, FilterMultipleValues)
{
    constexpr int        buffer_length = 10;
    BoxSecondOrderFilter filter("filter", nullptr);
    std::vector<double>  values(buffer_length);
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