//! @file
//! @brief File with unit tests of FIRFilter class.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "firFirstOrderFilter.h"
#include "staticJson.h"

using namespace vslib;

class FIRFirstOrderFilterTest : public ::testing::Test
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

    void setValues(FIRFirstOrderFilter& filter, std::array<double, 2>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.coefficients.setJsonValue(values);
        filter.coefficients.synchroniseWriteBuffer();
        BufferSwitch::flipState();
    }
};

//! Checks that a FIRFirstOrderFilter object can be constructed
TEST_F(FIRFirstOrderFilterTest, FilterDefaultConstruction)
{
    FIRFirstOrderFilter filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
}

//! Checks that a FIRFirstOrderFilter object can filter provided value
TEST_F(FIRFirstOrderFilterTest, FilterSingleValue)
{
    FIRFirstOrderFilter   filter("filter");
    std::array<double, 2> coefficient_array{0.2, 0.8};
    setValues(filter, coefficient_array);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-3);
}

//! Checks that a FIRFirstOrderFilter object can filter a number of provided values
TEST_F(FIRFirstOrderFilterTest, FilterMultipleValues)
{
    constexpr int         filter_length = 3;
    FIRFirstOrderFilter   filter("filter");
    std::array<double, 2> coefficient_array{0.2, 0.8};
    setValues(filter, coefficient_array);

    std::array<double, filter_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[2]), inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1], 1e-3);
}

//! Checks the behaviour of second-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFirstOrderFilterTest, FilterBMeasData)
{
    FIRFirstOrderFilter   filter("filter");
    std::array<double, 2> coefficient_array{0.5, 0.5};   // calculated by fir1 Matlab function with n=1, and Wn=0.5
    setValues(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fo_fir.csv";

    std::ifstream inputs_file(inputs_path);
    std::ifstream outputs_file(outputs_path);
    ASSERT_TRUE(inputs_file.is_open());
    ASSERT_TRUE(outputs_file.is_open());

    std::string input_str;
    std::string output_str;

    while (getline(inputs_file, input_str) && getline(outputs_file, output_str))
    {
        auto const input_value         = std::stod(input_str);
        auto const matlab_output_value = std::stod(output_str);

        double const filtered_value = filter.filter(input_value);
        auto const   relative       = (matlab_output_value - filtered_value) / matlab_output_value;
        EXPECT_NEAR(relative, 0.0, 1e-3);   // at least 1e-3 relative precision
    }
    inputs_file.close();
    outputs_file.close();
}
