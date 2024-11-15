//! @file
//! @brief File with unit tests of FIRFilter class.
//! @author Dominik Arominski

#include <filesystem>
#include <gtest/gtest.h>

#include "csv.hpp"
#include "firFilter.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;
using namespace csv;

class FIRFilterTest : public ::testing::Test
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

    template<int64_t FilterOrder>
    void setValues(FIRFilter<FilterOrder>& filter, std::array<double, FilterOrder + 1>& parameter_values)
    {
        StaticJson values = parameter_values;
        filter.coefficients.setJsonValue(values);
        filter.verifyParameters();
        filter.flipBufferState();
        filter.coefficients.syncWriteBuffer();
    }
};

//! Checks that a FIRFilter object can be constructed
TEST_F(FIRFilterTest, FilterDefaultConstruction)
{
    RootComponent root;
    FIRFilter<2>  filter("filter", root);
    EXPECT_EQ(filter.getName(), "filter");
}

//! Checks that a FIRFilter object can filter provided value
TEST_F(FIRFilterTest, FilterSingleValue)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 3;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{0.05, 0.8, 0.025, 0.057};
    setValues<filter_order>(filter, coefficient_array);
    for (int index = 0; index < filter_length; index++)
    {
        EXPECT_NEAR(filter.coefficients[index], coefficient_array[index], 1e-9);
    }

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-6);
}

//! Checks that a partial template specialization (1st order) object can filter provided value
TEST_F(FIRFilterTest, FirstOrderFilterSingleValue)
{
    RootComponent         root;
    FIRFilter<1>          filter("filter", root);
    std::array<double, 2> coefficient_array{0.2, 0.8};
    setValues<1>(filter, coefficient_array);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-6);
}

//! Checks that a partial template specialization (2nd order) object can filter provided value
TEST_F(FIRFilterTest, SecondOrderFilterSingleValue)
{
    RootComponent         root;
    FIRFilter<2>          filter("filter", root);
    std::array<double, 3> coefficient_array{0.05, 0.8, 0.15};
    setValues<2>(filter, coefficient_array);

    double input = 3.14159;
    EXPECT_NEAR(filter.filter(input), input * coefficient_array[0], 1e-6);
}

//! Checks that a FIRFilter object can filter a number of provided values
TEST_F(FIRFilterTest, FilterMultipleValues)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 7;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{0.1, 0.8, 0.05, 0.05};
    setValues<filter_order>(filter, coefficient_array);

    std::array<double, filter_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-6);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-6);
    EXPECT_NEAR(
        filter.filter(inputs[2]),
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-6
    );
}

//! Checks that a partial template specialization (1st order) object can filter a number of provided values
TEST_F(FIRFilterTest, FirstOrderFilterMultipleValues)
{
    RootComponent         root;
    constexpr int         inputs_length = 3;
    FIRFilter<1>          filter("filter", root);
    std::array<double, 2> coefficient_array{0.2, 0.8};
    setValues<1>(filter, coefficient_array);

    std::array<double, inputs_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-6);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-6);
    EXPECT_NEAR(filter.filter(inputs[2]), inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1], 1e-6);
}

//! Checks that a partial template specialization (2nd order) object can filter a number of provided values
TEST_F(FIRFilterTest, SecondOrderFilterMultipleValues)
{
    RootComponent         root;
    constexpr int         inputs_length = 3;
    FIRFilter<2>          filter("filter", root);
    std::array<double, 3> coefficient_array{0.15, 0.8, 0.05};
    setValues<2>(filter, coefficient_array);

    std::array<double, inputs_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-4);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-6);
    EXPECT_NEAR(
        filter.filter(inputs[2]),
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-6
    );
}

//! Checks that a FIRFilter object filters correctly a number of provided values larger than the number of coefficients
TEST_F(FIRFilterTest, FilterMultipleValuesWrapAround)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 1;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{0.2, 0.8};
    setValues<filter_order>(filter, coefficient_array);

    std::array<double, filter_length + 1> inputs{3.14159, 3.14159 * 2, 3.14159 * 3};
    EXPECT_NEAR(filter.filter(inputs[0]), inputs[0] * coefficient_array[0], 1e-4);
    EXPECT_NEAR(filter.filter(inputs[1]), inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-6);
    EXPECT_NEAR(filter.filter(inputs[2]), inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1], 1e-6);
}

//! Checks that a FIRFilter object can filter an array of inputs at once
TEST_F(FIRFilterTest, FilterEntireArray)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 3;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{0.1, 0.4, 0.4, 0.1};
    setValues<filter_order>(filter, coefficient_array);

    std::array<double, filter_length> inputs{3.14159, 3.14159 * 2, 3.14159 * 3, 3.14159 * 4};
    auto const                        output = filter.filter(inputs);
    EXPECT_NEAR(output[0], inputs[0] * coefficient_array[0], 1e-6);
    EXPECT_NEAR(output[1], inputs[1] * coefficient_array[0] + inputs[0] * coefficient_array[1], 1e-6);
    EXPECT_NEAR(
        output[2],
        inputs[2] * coefficient_array[0] + inputs[1] * coefficient_array[1] + inputs[0] * coefficient_array[2], 1e-6
    );
    EXPECT_NEAR(
        output[3],
        inputs[3] * coefficient_array[0] + inputs[2] * coefficient_array[1] + inputs[1] * coefficient_array[2]
            + inputs[0] * coefficient_array[3],
        1e-6
    );
}

//! Checks the behaviour of third-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, FilterBMeasDataThirdOrder)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 3;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{
        0.01674, 0.48326, 0.48326, 0.01674};   // calculated by fir1 Matlab function with n=3, and Wn=0.5
    setValues<filter_order>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fir_3_0_5.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader inputs_file(inputs_path.c_str(), format);
    CSVReader outputs_file(outputs_path.c_str(), format);

    auto inputs_line  = inputs_file.begin();
    auto outputs_line = outputs_file.begin();

    while (inputs_line != inputs_file.end() && outputs_line != outputs_file.end())
    {
        const auto input_value         = (*inputs_line)[0].get<double>();
        const auto matlab_output_value = (*outputs_line)[0].get<double>();

        const double filtered_value = filter.filter(input_value);
        const double relative       = (matlab_output_value - filtered_value) / matlab_output_value;
        ASSERT_NEAR(relative, 0.0, 3e-4);   // at least 3e-4 relative precision

        ++inputs_line;
        ++outputs_line;
    }
}

//! Checks the behaviour of fifth-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, FilterBMeasDataSeventhOrder)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 7;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{
        -7.776e-3, 6.445e-2, 4.433e-1,
        4.433e-1,  6.445e-2, -7.776e-3};   // calculated by fir1 Matlab function with n=5, and Wn=0.5
    setValues<filter_order>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fir_5_0_5.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader inputs_file(inputs_path.c_str(), format);
    CSVReader outputs_file(outputs_path.c_str(), format);

    auto inputs_line  = inputs_file.begin();
    auto outputs_line = outputs_file.begin();

    while (inputs_line != inputs_file.end() && outputs_line != outputs_file.end())
    {
        const auto input_value         = (*inputs_line)[0].get<double>();
        const auto matlab_output_value = (*outputs_line)[0].get<double>();

        const double filtered_value = filter.filter(input_value);
        const double relative       = (matlab_output_value - filtered_value) / matlab_output_value;
        ASSERT_NEAR(relative, 0.0, 1e-4);   // at least 1e-4 relative precision

        ++inputs_line;
        ++outputs_line;
    }
}

//! Checks the behaviour of tenth-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, FilterBMeasDataTenthOrder)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 10;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{
        5.060e-3, 0.0,  -4.194e-2, 0.0, 2.885e-1, 4.968e-1,
        2.885e-1, 0.00, -4.194e-2, 0.0, 5.060e-3};   // calculated by fir1 Matlab function with n=10, and Wn=0.5
    setValues<filter_order>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fir_10_0_5.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader inputs_file(inputs_path.c_str(), format);
    CSVReader outputs_file(outputs_path.c_str(), format);

    auto inputs_line  = inputs_file.begin();
    auto outputs_line = outputs_file.begin();

    while (inputs_line != inputs_file.end() && outputs_line != outputs_file.end())
    {
        const auto input_value         = (*inputs_line)[0].get<double>();
        const auto matlab_output_value = (*outputs_line)[0].get<double>();

        const double filtered_value = filter.filter(input_value);
        double       relative;
        if (matlab_output_value != 0)
        {
            relative = (matlab_output_value - filtered_value) / matlab_output_value;
        }
        else
        {
            relative = (matlab_output_value - filtered_value);
        }
        ASSERT_NEAR(relative, 0.0, 1e-4);   // at least 1e-4 relative precision

        ++inputs_line;
        ++outputs_line;
    }
}

//! Checks the behaviour of fourth-order low-pass FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, LowPassFilterBMeasDataFourthOrder)
{
    RootComponent           root;
    constexpr int           filter_order  = 4;
    constexpr int           filter_length = filter_order + 1;
    FIRFilter<filter_order> filter("filter", root);
    // calculated with coefficients = designLowpassFIR(FilterOrder=2,CutoffFrequency=0.5,Window="hann"); command in
    // Matlab:
    std::array<double, filter_length> coefficient_array{0, 0.1945, 0.6110, 0.1945, 0};
    setValues<filter_order>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    // the output from:
    // firFilt = dsp.FIRFilter(NumeratorSource="Input port");
    // firFilt(input_data, coefficients);
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_low-pass_fir_4_0_5.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader inputs_file(inputs_path.c_str(), format);
    CSVReader outputs_file(outputs_path.c_str(), format);

    auto inputs_line  = inputs_file.begin();
    auto outputs_line = outputs_file.begin();

    while (inputs_line != inputs_file.end() && outputs_line != outputs_file.end())
    {
        const auto input_value         = (*inputs_line)[0].get<double>();
        const auto matlab_output_value = (*outputs_line)[0].get<double>();

        const double filtered_value = filter.filter(input_value);
        double       relative;
        if (matlab_output_value != 0)
        {
            relative = (matlab_output_value - filtered_value) / matlab_output_value;
        }
        else
        {
            relative = (matlab_output_value - filtered_value);
        }
        ASSERT_NEAR(relative, 0.0, 5e-5);   // at least 1e-6 relative precision

        ++inputs_line;
        ++outputs_line;
    }
}

//! Checks the behaviour of an 81st-order FIR filter on a real data coming from
//! GPS power converter, and compared with filtering in Matlab
TEST_F(FIRFilterTest, FilterBMeasData81stOrder)
{
    RootComponent                     root;
    constexpr int                     filter_order  = 81;
    constexpr int                     filter_length = filter_order + 1;
    FIRFilter<filter_order>           filter("filter", root);
    std::array<double, filter_length> coefficient_array{
        0.000444601818173842,  -0.000463739605799769, -0.000499996477092942, 0.000554592463829041,
        0.00062877993267011,   -0.000723849715913591, -0.00084113894367772,  0.000982040919065611,
        0.00114801745795324,   -0.00134061421815078,  -0.00156147967635684,  0.00181238858594903,
        0.002095270978624,     -0.00241224807836754,  -0.00276567690571181,  0.00315820590453566,
        0.00359284468191405,   -0.00407305200053809,  -0.00460284763224963,  0.00518695576564101,
        0.00583099066065692,   -0.00654169962825014,  -0.007327284932108,    0.00819783608164237,
        0.00916591924306309,   -0.0102473946129353,   -0.0114625716864479,   0.0128378774997698,
        0.0144083249797791,    -0.0162212682835868,   -0.0183423029738756,   0.0208648912798552,
        0.0239267799965757,    -0.0277395506429421,   -0.0326454441227168,   0.0392361726881932,
        0.0486301436738795,    -0.0632249182823701,   -0.0892555632549734,   0.149586395115587,
        0.450003611946707,     0.450003611946707,     0.149586395115587,     -0.0892555632549734,
        -0.0632249182823701,   0.0486301436738795,    0.0392361726881932,    -0.0326454441227168,
        -0.0277395506429421,   0.0239267799965757,    0.0208648912798552,    -0.0183423029738756,
        -0.0162212682835868,   0.0144083249797791,    0.0128378774997698,    -0.0114625716864479,
        -0.0102473946129353,   0.00916591924306309,   0.00819783608164237,   -0.007327284932108,
        -0.00654169962825014,  0.00583099066065692,   0.00518695576564101,   -0.00460284763224963,
        -0.00407305200053809,  0.00359284468191405,   0.00315820590453566,   -0.00276567690571181,
        -0.00241224807836754,  0.002095270978624,     0.00181238858594903,   -0.00156147967635684,
        -0.00134061421815078,  0.00114801745795324,   0.000982040919065611,  -0.00084113894367772,
        -0.000723849715913591, 0.00062877993267011,   0.000554592463829041,  -0.000499996477092942,
        -0.000463739605799769, 0.000444601818173842};   // calculated by fir1 Matlab function with n=80, and Wn=0.5
    setValues<filter_order>(filter, coefficient_array);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path inputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_shortened.csv";
    std::filesystem::path outputs_path
        = "components/inputs/RPACZ.197.YGPS.RDS.3000.B_MEAS_2020-10-08_14-06-11_fir_80_0_5.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader inputs_file(inputs_path.c_str(), format);
    CSVReader outputs_file(outputs_path.c_str(), format);

    auto inputs_line  = inputs_file.begin();
    auto outputs_line = outputs_file.begin();

    while (inputs_line != inputs_file.end() && outputs_line != outputs_file.end())
    {
        const auto input_value         = (*inputs_line)[0].get<double>();
        const auto matlab_output_value = (*outputs_line)[0].get<double>();

        const double filtered_value = filter.filter(input_value);
        double       relative;
        if (matlab_output_value != 0)
        {
            relative = (matlab_output_value - filtered_value) / matlab_output_value;
        }
        else
        {
            relative = (matlab_output_value - filtered_value);
        }
        ASSERT_NEAR(relative, 0.0, 1e-6);   // at least 1e-4 relative precision

        ++inputs_line;
        ++outputs_line;
    }
}