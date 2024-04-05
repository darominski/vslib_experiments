//! @file
//! @brief File with unit tests of IIRFilter class.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "iirFilter.h"
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
        ParameterRegistry& parameter_registry = ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    //! Helper method to set numerator values
    template<size_t N>
    void setNumeratorValues(IIRFilter<N>& filter, std::array<double, N>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.numerator.setJsonValue(values);
        filter.flipBufferState();
        filter.numerator.syncWriteBuffer();
    }

    //! Helper method to set denominator values
    template<size_t N>
    void setDenominatorValues(IIRFilter<N>& filter, std::array<double, N>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.denominator.setJsonValue(values);
        filter.flipBufferState();
        filter.denominator.syncWriteBuffer();
    }
};

//! Checks that a IIRFilter object can be constructed
TEST_F(IIRFilterTest, FilterDefaultConstruction)
{
    IIRFilter<2> filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
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
    EXPECT_NEAR(filter.filter(input), input * numerator_values[0], 1e-6);
}

//! Checks that a partial template specialization (1st order) IIRFilter object can filter provided value.
//! For a single input, the IIR will still behave like an FIR.
TEST_F(IIRFilterTest, FirstOrderFilterSingleValueSetDenominator)
{
    constexpr int         inputs_length = 3;
    IIRFilter<2>          filter("filter");
    std::array<double, 2> numerator_values{0.3, 0.7};
    setNumeratorValues(filter, numerator_values);
    std::array<double, 2> denominator_values{1.0, -0.37};   // from Matlab: Butterworth IIR filter
    setDenominatorValues(filter, denominator_values);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * numerator_values[0], 1e-6);
}

//! Checks that a FIRFilter object can filter a number of provided values, without wrapping around the buffers
TEST_F(IIRFilterTest, FirstOrderFilterMultipleValues)
{
    constexpr int         input_length = 3;
    IIRFilter<2>          filter("filter");
    std::array<double, 2> numerator_values{0.2, 0.8};
    setNumeratorValues(filter, numerator_values);
    std::array<double, 2> denominator_values{1.0, -0.37};
    setDenominatorValues(filter, denominator_values);

    std::array<double, input_length> inputs{3.14159 * 0.5, 3.14159 * 1, 3.14159 * 1.5};
    std::array<double, input_length> outputs{0};

    outputs[0] = filter.filter(inputs[0]);
    EXPECT_NEAR(outputs[0], inputs[0] * numerator_values[0], 1e-6);

    outputs[1] = filter.filter(inputs[1]);
    EXPECT_NEAR(
        outputs[1],
        inputs[1] * numerator_values[0] + inputs[0] * numerator_values[1] - outputs[0] * denominator_values[1], 1e-6
    );

    outputs[2] = filter.filter(inputs[2]);
    EXPECT_NEAR(
        outputs[2],
        inputs[2] * numerator_values[0] + inputs[1] * numerator_values[1] - (outputs[1] * denominator_values[1]), 1e-6
    );
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
    EXPECT_NEAR(filter.filter(input), input * numerator_values[0], 1e-6);
}

//! Checks that a IIRFilter object can filter a number of provided values, without wrapping around the buffers
TEST_F(IIRFilterTest, FilterMultipleValues)
{
    constexpr int                     filter_length = 4;
    IIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> numerator_values{0.1, 0.8, 0.05, 0.05};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.0, -0.37, 0.20};
    setDenominatorValues<filter_length>(filter, denominator_values);

    std::array<double, filter_length> inputs{3.14159 * 0.5, 3.14159 * 1, 3.14159 * 1.5};
    std::array<double, filter_length> outputs{0};

    outputs[0] = filter.filter(inputs[0]);
    EXPECT_NEAR(outputs[0], inputs[0] * numerator_values[0], 1e-6);

    outputs[1] = filter.filter(inputs[1]);
    EXPECT_NEAR(
        outputs[1],
        inputs[1] * numerator_values[0] + inputs[0] * numerator_values[1] - outputs[0] * denominator_values[1], 1e-6
    );

    outputs[2] = filter.filter(inputs[2]);
    EXPECT_NEAR(
        outputs[2],
        inputs[2] * numerator_values[0] + inputs[1] * numerator_values[1] + inputs[0] * numerator_values[2]
            - (outputs[1] * denominator_values[1] + outputs[0] * denominator_values[2]),
        1e-6
    );
}

//! Checks that a IIRFilter object can filter a number of provided values, with buffer wrap-around
TEST_F(IIRFilterTest, FilterMultipleValuesBufferWrapAround)
{
    constexpr int                     filter_length = 3;
    IIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> numerator_values{0.1, 0.8, 0.1};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.0, -0.37, 0.20};
    setDenominatorValues<filter_length>(filter, denominator_values);

    double const                     pi           = 3.14159;
    constexpr int                    array_length = 5;
    std::array<double, array_length> inputs{pi * 0.5, pi, pi * 1.5, pi * 2, pi * 2.5};
    std::array<double, array_length> outputs{0};

    outputs[0] = filter.filter(inputs[0]);
    EXPECT_NEAR(outputs[0], inputs[0] * numerator_values[0], 1e-6);

    outputs[1] = filter.filter(inputs[1]);
    double expected_value
        = inputs[1] * numerator_values[0] + inputs[0] * numerator_values[1] - outputs[0] * denominator_values[1];
    EXPECT_NEAR((expected_value - outputs[1]) / expected_value, 0.0, 1e-6);

    outputs[2]     = filter.filter(inputs[2]);
    expected_value = inputs[2] * numerator_values[0] + inputs[1] * numerator_values[1] + inputs[0] * numerator_values[2]
                     - (outputs[1] * denominator_values[1] + outputs[0] * denominator_values[2]);
    EXPECT_NEAR((expected_value - outputs[2]) / expected_value, 0.0, 1e-6);

    outputs[3]     = filter.filter(inputs[3]);
    expected_value = inputs[3] * numerator_values[0] + inputs[2] * numerator_values[1] + inputs[1] * numerator_values[2]
                     - (outputs[2] * denominator_values[1] + outputs[1] * denominator_values[2]);
    EXPECT_NEAR((expected_value - outputs[3]) / expected_value, 0.0, 1e-6);

    outputs[4]     = filter.filter(inputs[4]);
    expected_value = inputs[4] * numerator_values[0] + inputs[3] * numerator_values[1] + inputs[2] * numerator_values[2]
                     - (outputs[3] * denominator_values[1] + outputs[2] * denominator_values[2]);
    EXPECT_NEAR((expected_value - outputs[4]) / expected_value, 0.0, 1e-6);
}

//! Checks that a IIRFilter object can filter a number of provided values, with buffer wrap-around
TEST_F(IIRFilterTest, FilterEntireArrayCompareWithMatlab)
{
    constexpr int                     filter_length = 3;
    IIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> numerator_values{0.1, 0.8, 0.1};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.0, -0.37, 0.20};
    setDenominatorValues<filter_length>(filter, denominator_values);

    double const                     pi           = 3.14159;
    constexpr int                    array_length = 5;
    std::array<double, array_length> inputs{pi * 0.5, pi, pi * 1.5, pi * 2, pi * 2.5};
    std::array<double, array_length> expected_values{0.1571, 1.6289, 3.7129, 5.7604, 7.6719};
    auto const                       filtered_values = filter.filter(inputs);

    for (int index = 0; index < array_length; index++)
    {
        EXPECT_NEAR((expected_values[index] - filtered_values[index]) / expected_values[index], 0.0, 2e-4);
    }
}

//! Checks the behaviour of second-order Butterworth IIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(IIRFilterTest, ButterIIRFilterBMeasSecondOrder)
{
    constexpr int            filter_length = 3;
    IIRFilter<filter_length> filter("filter", nullptr);
    // Matlab output and coefficients come from executing:
    // [b,a] = butter(2, 0.4);
    // iirFilt = dsp.IIRFilter('Numerator', b, 'Denominator', a);
    // iirFilt(input_data);
    std::array<double, filter_length> numerator_values{2.0657e-1, 4.1314e-1, 2.0657e-1};
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

    while (getline(inputs_file, input_str) && getline(outputs_file, output_str))
    {
        auto const input_value         = std::stod(input_str);
        auto const matlab_output_value = std::stod(output_str);

        double const filtered_value = filter.filter(input_value);

        double const relative = (matlab_output_value - filtered_value) / matlab_output_value;
        ASSERT_NEAR(relative, 0.0, 2e-4);   // at least 0.02% relative precision
    }
    inputs_file.close();
    outputs_file.close();
}

//! Checks the behaviour of a tenth-order Chebyshev Type II IIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(IIRFilterTest, ChebyIIRFilterBMeasTenthOrder)
{
    constexpr int            filter_length = 11;
    IIRFilter<filter_length> filter("filter", nullptr);
    // Matlab output and coefficients come from executing:
    // [b,a] = cheby1(10, 0.5, 0.5);
    // iirFilt = dsp.IIRFilter('Numerator', b, 'Denominator', a);
    // iirFilt(input_data);
    std::array<double, filter_length> numerator_values{2.89645E-03, 2.89645E-02, 1.30340E-01, 3.47574E-01,
                                                       6.08254E-01, 7.29904E-01, 6.08254E-01, 3.47574E-01,
                                                       1.30340E-01, 2.89645E-02, 2.89645E-03};
    setNumeratorValues<filter_length>(filter, numerator_values);
    std::array<double, filter_length> denominator_values{1.00000E00,  -3.12098E-15, 1.34038E00,  -3.19478E-15,
                                                         5.45354E-01, -8.28580E-16, 7.70412E-02, -1.38675E-17,
                                                         3.16548E-03, 1.58106E-17,  1.67788E-05};
    setDenominatorValues<filter_length>(filter, denominator_values);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_inputs.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPOPB.245.BR23.RMPS_B_MEAS_2023-11-17_09-32_iir_butter_10.csv";

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

        double const relative = (matlab_output_value - filtered_value) / matlab_output_value;
        ASSERT_NEAR(relative, 0.0, 5e-4);   // at least 0.05% relative precision
    }
    inputs_file.close();
    outputs_file.close();
}
