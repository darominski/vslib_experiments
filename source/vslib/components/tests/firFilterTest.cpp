//! @file
//! @brief File with unit tests of FIRFilter class.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "firFilter.h"
#include "staticJson.h"

using namespace vslib;

class FIRFilterTest : public ::testing::Test
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

    template<size_t N>
    void setValues(FIRFilter<N>& filter, std::array<double, N>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.coefficients.setJsonValue(values);
        filter.coefficients.synchroniseWriteBuffer();
        BufferSwitch::flipState();
    }
};

//! Checks that a FIRFilter object can be constructed
TEST_F(FIRFilterTest, FilterDefaultConstruction)
{
    FIRFilter<2> filter("filter");
    EXPECT_EQ(filter.getName(), "filter");
}

//! Checks that a FIRFilter object can filter provided value
TEST_F(FIRFilterTest, FilterSingleValue)
{
    constexpr int                     filter_length = 4;
    FIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> coefficient_array{0.05, 0.8, 0.025, 0.025};
    setValues<filter_length>(filter, coefficient_array);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-3);
}

//! Checks that a partial template specialization (1st order) object can filter provided value
TEST_F(FIRFilterTest, FirstOrderFilterSingleValue)
{
    FIRFilter<2>          filter("filter");
    std::array<double, 2> coefficient_array{0.2, 0.8};
    setValues(filter, coefficient_array);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-3);
}

//! Checks that a partial template specialization (2nd order) object can filter provided value
TEST_F(FIRFilterTest, SecondOrderFilterSingleValue)
{
    FIRFilter<3>          filter("filter");
    std::array<double, 3> coefficient_array{0.05, 0.8, 0.15};
    setValues(filter, coefficient_array);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-3);
}

//! Checks that a FIRFilter object can filter a number of provided values
TEST_F(FIRFilterTest, FilterMultipleValues)
{
    constexpr int                     filter_length = 4;
    FIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> coefficient_array{0.1, 0.8, 0.05, 0.05};
    setValues<filter_length>(filter, coefficient_array);

    std::array<double, filter_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-3);
    EXPECT_NEAR(
        filter.filter(inputs[2]),
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-3
    );
}

//! Checks that a partial template specialization (1st order) object can filter a number of provided values
TEST_F(FIRFilterTest, FirstOrderFilterMultipleValues)
{
    constexpr int         inputs_length = 3;
    FIRFilter<2>          filter("filter");
    std::array<double, 2> coefficient_array{0.2, 0.8};
    setValues(filter, coefficient_array);

    std::array<double, inputs_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[2]), inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1], 1e-3);
}

//! Checks that a partial template specialization (2nd order) object can filter a number of provided values
TEST_F(FIRFilterTest, SecondOrderFilterMultipleValues)
{
    constexpr int         inputs_length = 3;
    FIRFilter<3>          filter("filter");
    std::array<double, 3> coefficient_array{0.15, 0.8, 0.05};
    setValues(filter, coefficient_array);

    std::array<double, inputs_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-3);
    EXPECT_NEAR(
        filter.filter(inputs[2]),
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-3
    );
}

//! Checks that a FIRFilter object filters correctly a number of provided values larger than the number of coefficients
TEST_F(FIRFilterTest, FilterMultipleValuesWrapAround)
{
    constexpr int                     filter_length = 2;
    FIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> coefficient_array{0.2, 0.8};
    setValues<filter_length>(filter, coefficient_array);

    std::array<double, filter_length + 1> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-3);
    EXPECT_NEAR(filter.filter(inputs[2]), inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1], 1e-3);
}

//! Checks that a FIRFilter object can filter an array of inputs at once
TEST_F(FIRFilterTest, FilterEntireArray)
{
    constexpr int                     filter_length = 4;
    FIRFilter<filter_length>          filter("filter", nullptr);
    std::array<double, filter_length> coefficient_array{0.1, 0.4, 0.4, 0.1};
    setValues<filter_length>(filter, coefficient_array);

    std::array<double, filter_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3, 3.14159 * 4};
    auto const                        output = filter.filter(inputs);
    EXPECT_NEAR(output[0], inputs[0] * coefficient_array[0], 1e-3);
    EXPECT_NEAR(output[1], inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-3);
    EXPECT_NEAR(
        output[2],
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-3
    );
    EXPECT_NEAR(
        output[3],
        inputs[3] * coefficient_array[0] + inputs[2] * coefficient_array[1] + inputs[1] * coefficient_array[2]
            + inputs[0] * coefficient_array[3],
        1e-3
    );
}

//! Checks the behaviour of second-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, FilterBMeasDataSecondOrder)
{
    constexpr int                     filter_length = 4;
    FIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> coefficient_array{
        0.01674, 0.48326, 0.48326, 0.01674};   // calculated by fir1 Matlab function with n=3, and Wn=0.5
    setValues<filter_length>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fir_3_0_5.csv";

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

//! Checks the behaviour of fifth-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, FilterBMeasDataFifthOrder)
{
    constexpr int                     filter_length = 6;
    FIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> coefficient_array{
        -7.776e-3, 6.445e-2, 4.433e-1,
        4.433e-1,  6.445e-2, -7.776e-3};   // calculated by fir1 Matlab function with n=5, and Wn=0.5
    setValues<filter_length>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fir_5_0_5.csv";

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

//! Checks the behaviour of tenth-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, FilterBMeasDataTenthOrder)
{
    constexpr int                     filter_length = 11;
    FIRFilter<filter_length>          filter("filter");
    std::array<double, filter_length> coefficient_array{
        5.060e-3, 0.0,  -4.194e-2, 0.0, 2.885e-1, 4.968e-1,
        2.885e-1, 0.00, -4.194e-2, 0.0, 5.060e-3};   // calculated by fir1 Matlab function with n=10, and Wn=0.5
    setValues<filter_length>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fir_10_0_5.csv";

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

//! Checks the behaviour of fourth-order low-pass FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, LowPassFilterBMeasDataFourthOrder)
{
    constexpr int            filter_length = 11;
    FIRFilter<filter_length> filter("filter");
    // calculated with coefficients = designLowpassFIR(FilterOrder=2,CutoffFrequency=0.5,Window="hann"); command in
    // Matlab:
    std::array<double, filter_length> coefficient_array{0, 0.1945, 0.6110, 0.1945, 0};
    setValues<filter_length>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    // the output from:
    // firFilt = dsp.FIRFilter(NumeratorSource="Input port");
    // firFilt(input_data, coefficients);
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_low-pass_fir_4_0_5.csv";

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
        double       relative;
        if (matlab_output_value == 0)
        {
            relative = (matlab_output_value - filtered_value);
        }
        else
        {
            relative = (matlab_output_value - filtered_value) / matlab_output_value;
        }
        EXPECT_NEAR(relative, 0.0, 1e-3);   // at least 1e-2 relative precision
    }
    inputs_file.close();
    outputs_file.close();
}
