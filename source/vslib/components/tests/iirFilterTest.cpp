//! @file
//! @brief File with unit tests of FIRFilter class.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "IIRFilter.h"
#include "componentRegistry.h"
#include "staticJson.h"

using namespace vslib;

class IIRFilterTest : public ::testing::Test
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
    template<size_t N>
    void setNumeratorValues(IIRFilter<N>& filter, std::array<double, N>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.numerator.setJsonValue(values);
        filter.numerator.synchroniseWriteBuffer();
        BufferSwitch::flipState();
        filter.numerator.synchroniseReadBuffers();
    }

    //! Helper method to set denominator values
    template<size_t N>
    void setDenominatorValues(IIRFilter<N>& filter, std::array<double, N>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.denominator.setJsonValue(values);
        filter.denominator.synchroniseWriteBuffer();
        BufferSwitch::flipState();
        filter.denominator.synchroniseReadBuffers();
    }
};

//! Checks that a IIRFilter object can be constructed
TEST_F(IIRFilterTest, FilterDefaultConstruction)
{
    IIRFilter<1> filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
    EXPECT_EQ(filter.getMaxInputValue(), 1e6);
}

//! Checks that a IIRFilter object can be constructed with non-default parameters
TEST_F(IIRFilterTest, FilterNonDefaultConstruction)
{
    IIRFilter<1> filter("filter", nullptr, 1e4);
    EXPECT_EQ(filter.getName(), "filter");
    EXPECT_EQ(filter.getMaxInputValue(), 1e4);
}

//! Checks that a IIRFilter object can filter provided value.
//! Without setting denominator values it should behave like an FIR.
TEST_F(IIRFilterTest, FilterSingleValue)
{
    constexpr int                     filter_length = 3;
    IIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> numerator_values{0.1, 0.8, 0.1};
    setNumeratorValues<filter_length>(filter, numerator_values);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * numerator_values[0], 1e-3);
}

//! Checks that a IIRFilter object can filter provided value. For a single input,
//! the IIR will still behave like an FIR.
TEST_F(IIRFilterTest, FilterSingleValueSetDenominator)
{
    constexpr int                     filter_length = 3;
    IIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> numerator_values{0.1, 0.8, 0.1};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.0, -0.37, 0.20};   // from Matlab: Butterworth IIR filter
    setDenominatorValues<filter_length>(filter, denominator_values);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * numerator_values[0], 1e-3);
}

//! Checks that a FIRFilter object can filter a number of provided values
TEST_F(IIRFilterTest, FilterMultipleValues)
{
    constexpr int                     filter_length = 3;
    IIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> numerator_values{0.1, 0.8, 0.1};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.0, -0.37, 0.20};
    setDenominatorValues<filter_length>(filter, denominator_values);

    std::array<double, filter_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    std::array<double, filter_length> outputs{0};

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
        inputs[2] * numerator_values[0] + inputs[1] * numerator_values[1] + inputs[0] * numerator_values[2]
            - (outputs[1] * denominator_values[1] + outputs[0] * denominator_values[2]),
        1e-3
    );
}

//! Checks the behaviour of second-order Butterworth IIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(IIRFilterTest, ButterIIRFilterBMeasSecondOrder)
{
    constexpr int            filter_length = 3;
    IIRFilter<filter_length> filter("filter", nullptr, 2e4);
    // Matlab output and coefficients come from executing:
    // [b,a] = butter(2, 0.4);
    // iirFilt = dsp.IIRFilter('Numerator', b, 'Denominator', a);
    // iirFilt(input_data);
    std::array<double, filter_length> numerator_values{2.0657e-1, 4.1314e-1, 2.057e-1};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.0, -3.6953e-1, 1.9582e-1};
    setDenominatorValues<filter_length>(filter, denominator_values);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_inputs.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_iir_butter_2.csv";

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
        ASSERT_NEAR(relative, 0.0, 4e-2);   // at least 4% relative consistency, limited by casting
    }
    inputs_file.close();
    outputs_file.close();
}

//! Checks the behaviour of a tenth-order Chebyshev Type II IIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(IIRFilterTest, ChebyIIRFilterBMeasTenthOrder)
{
    constexpr int            filter_length = 11;
    IIRFilter<filter_length> filter("filter", nullptr, 1e3);
    // Matlab output and coefficients come from executing:
    // [b,a] = cheby1(10, 0.5, 0.5);
    // iirFilt = dsp.IIRFilter('Numerator', b, 'Denominator', a);
    // iirFilt(input_data);
    std::array<double, filter_length> numerator_values{3.4877E-4, 3.4877E-3, 1.5695E-2, 4.1852E-2, 7.3241E-2, 8.7890E-2,
                                                       7.3241E-2, 4.1852E-2, 1.5695E-2, 3.4877E-3, 3.4877E-4};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.0000E0, -3.0128E0, 6.5992E0, -1.0166E1,  1.2378E1, -1.1971E1,
                                                         9.2990E0, -5.7129E0, 2.6817E0, -8.8487E-1, 1.6786E-1};
    setDenominatorValues<filter_length>(filter, denominator_values);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_inputs.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_iir_cheby1_10.csv";

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

        double const relative = abs(matlab_output_value - filtered_value) / matlab_output_value;
        ASSERT_NEAR(relative, 0.0, 0.2);   // at least 20% relative precision
    }
    inputs_file.close();
    outputs_file.close();
}