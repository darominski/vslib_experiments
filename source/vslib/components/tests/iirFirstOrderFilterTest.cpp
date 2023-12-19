//! @file
//! @brief File with unit tests of IIRFirstOrderFilter class.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "iirFirstOrderFilter.h"
#include "staticJson.h"

using namespace vslib;

class IIRFirstOrderFilterTest : public ::testing::Test
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

    //! Helper method to set numerator values
    void setNumeratorValues(IIRFirstOrderFilter& filter, std::array<double, 2>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.numerator.setJsonValue(values);
        filter.numerator.synchroniseWriteBuffer();
        BufferSwitch::flipState();
        filter.numerator.synchroniseReadBuffers();
    }

    //! Helper method to set denominator values
    void setDenominatorValues(IIRFirstOrderFilter& filter, std::array<double, 2>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.denominator.setJsonValue(values);
        filter.denominator.synchroniseWriteBuffer();
        BufferSwitch::flipState();
        filter.denominator.synchroniseReadBuffers();
    }
};

//! Checks that a IIRFilter object can be constructed
TEST_F(IIRFirstOrderFilterTest, FilterDefaultConstruction)
{
    IIRFirstOrderFilter filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
}

//! Checks that a IIRFilter object can filter provided value.
//! Without setting denominator values it should behave like an FIR.
TEST_F(IIRFirstOrderFilterTest, FilterSingleValue)
{
    IIRFirstOrderFilter   filter("filter");
    std::array<double, 2> numerator_values{0.2, 0.8};
    setNumeratorValues(filter, numerator_values);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * numerator_values[0], 1e-3);
}

//! Checks that a IIRFilter object can filter provided value. For a single input,
//! the IIR will still behave like an FIR.
TEST_F(IIRFirstOrderFilterTest, FilterSingleValueSetDenominator)
{
    constexpr int         inputs_length = 3;
    IIRFirstOrderFilter   filter("filter");
    std::array<double, 2> numerator_values{0.3, 0.7};
    setNumeratorValues(filter, numerator_values);
    std::array<double, 2> denominator_values{1.0, -0.37};   // from Matlab: Butterworth IIR filter
    setDenominatorValues(filter, denominator_values);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * numerator_values[0], 1e-3);
}

//! Checks that a FIRFilter object can filter a number of provided values, without wrapping around the buffers
TEST_F(IIRFirstOrderFilterTest, FilterMultipleValues)
{
    constexpr int         input_length = 3;
    IIRFirstOrderFilter   filter("filter");
    std::array<double, 2> numerator_values{0.2, 0.8};
    setNumeratorValues(filter, numerator_values);
    std::array<double, 2> denominator_values{1.0, -0.37};
    setDenominatorValues(filter, denominator_values);

    std::array<double, input_length> inputs{3.14159 * 0.5, 3.14159 * 1, 3.14159 * 1.5};
    std::array<double, input_length> outputs{0};

    outputs[0] = filter.filter(inputs[0]);
    EXPECT_NEAR(outputs[0], inputs[0] * numerator_values[0], 1e-3);

    outputs[1] = filter.filter(inputs[1]);
    EXPECT_NEAR(
        outputs[1],
        inputs[1] * numerator_values[0] + inputs[0] * numerator_values[1] - outputs[0] * denominator_values[1], 1e-3
    );

    outputs[2] = filter.filter(inputs[2]);
    EXPECT_NEAR(
        outputs[2],
        inputs[2] * numerator_values[0] + inputs[1] * numerator_values[1] - (outputs[1] * denominator_values[1]), 1e-3
    );
}

//! Checks the behaviour of second-order Butterworth IIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(IIRFirstOrderFilterTest, ButterIIRFilterBMeasSecondOrder)
{
    IIRFirstOrderFilter filter("filter");
    // Matlab output and coefficients come from executing:
    // [b,a] = butter(1, 0.4);
    // iirFilt = dsp.IIRFilter('Numerator', b, 'Denominator', a);
    // iirFilt(input_data);

    std::array<double, 2> numerator_values{0.42081, 0.42081};
    setNumeratorValues(filter, numerator_values);
    std::array<double, 2> denominator_values{1.0, -0.15838};
    setDenominatorValues(filter, denominator_values);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_inputs.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_fo_iir_butter.csv";

    std::ifstream inputs_file(inputs_path);
    std::ifstream outputs_file(outputs_path);
    ASSERT_TRUE(inputs_file.is_open());
    ASSERT_TRUE(outputs_file.is_open());

    std::string input_str;
    std::string output_str;

    int counter = 0;

    while (getline(inputs_file, input_str) && getline(outputs_file, output_str))
    {
        auto const input_value         = std::stod(input_str);
        auto const matlab_output_value = std::stod(output_str);

        double const filtered_value = filter.filter(input_value);

        double const relative = (matlab_output_value - filtered_value) / matlab_output_value;
        ASSERT_NEAR(relative, 0.0, 5e-3);   // at least 0.5% relative precision
    }
    inputs_file.close();
    outputs_file.close();
}