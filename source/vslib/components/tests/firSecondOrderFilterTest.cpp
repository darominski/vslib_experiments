//! @file
//! @brief File with unit tests of FIRFilter class.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "firSecondOrderFilter.h"
#include "staticJson.h"

using namespace vslib;

class FIRSecondOrderFilterTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        ComponentRegistry& component_registry = ComponentRegistry::instance();
        component_registry.clearRegistry();
        ParameterRegistry& parameter_registry = ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    void setValues(FIRSecondOrderFilter& filter, std::array<double, 3>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.coefficients.setJsonValue(values);
        filter.coefficients.synchroniseWriteBuffer();
        BufferSwitch::flipState();
    }
};

//! Checks that a FIRSecondOrderFilter object can be constructed
TEST_F(FIRSecondOrderFilterTest, FilterDefaultConstruction)
{
    FIRSecondOrderFilter filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
}

//! Checks that a FIRSecondOrderFilter object can filter provided value
TEST_F(FIRSecondOrderFilterTest, FilterSingleValue)
{
    FIRSecondOrderFilter  filter("filter");
    std::array<double, 3> coefficient_array{0.15, 0.8, 0.05};
    setValues(filter, coefficient_array);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-3);
}

//! Checks that a FIRSecondOrderFilter object can filter a number of provided values
TEST_F(FIRSecondOrderFilterTest, FilterMultipleValues)
{
    constexpr int         inputs_length = 3;
    FIRSecondOrderFilter  filter("filter");
    std::array<double, 3> coefficient_array{0.15, 0.8, 0.05};
    setValues(filter, coefficient_array);

    std::array<double, inputs_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-6);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-6);
    EXPECT_NEAR(
        filter.filter(inputs[2]),
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-6
    );
}


//! Checks that a FIRSecondOrderFilter object can filter a number of provided values larger than the number of
//! coefficients
TEST_F(FIRSecondOrderFilterTest, FilterMultipleValuesWrapAround)
{
    constexpr int         inputs_length = 4;
    FIRSecondOrderFilter  filter("filter");
    std::array<double, 3> coefficient_array{0.15, 0.8, 0.05};
    setValues(filter, coefficient_array);

    std::array<double, inputs_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3, 3.14159 * 4};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-6);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-6);
    EXPECT_NEAR(
        filter.filter(inputs[2]),
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-6
    );
    EXPECT_NEAR(
        filter.filter(inputs[3]),
        inputs[3] * coefficient_array[0] + inputs[2] * coefficient_array[1] + inputs[1] * coefficient_array[2], 1e-6
    );
}