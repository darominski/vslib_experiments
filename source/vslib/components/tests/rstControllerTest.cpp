//! @file
//! @brief File with unit tests of RST controller.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "rstController.h"

using namespace vslib;

class RSTControllerTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks that a default RSTController object can be default-constructed
//! and is initialized as expected
TEST_F(RSTControllerTest, RSTControllerDefaultConstruction)
{
    constexpr size_t controller_length = 3;

    RSTController<controller_length> rst("rst");

    EXPECT_FALSE(rst.isReady());

    for (int64_t index = 0; index < controller_length; index++)
    {
        EXPECT_EQ(rst.getR()[index], 0.0);
        EXPECT_EQ(rst.getS()[index], 0.0);
        EXPECT_EQ(rst.getT()[index], 0.0);
    }
}

//! Checks that the input histories can be updated and when enough of points are provided, the RST is ready to control
TEST_F(RSTControllerTest, RSTControllerUpdateInputHistories)
{
    constexpr size_t controller_length = 5;

    RSTController<controller_length> rst("rst");

    for (size_t index = 0; index < controller_length - 1; index++)
    {
        EXPECT_EQ(rst.isReady(), false);
        rst.updateInputHistories(index + 1, index);
    }
    // when the history buffers are filled: the RST should be ready
    EXPECT_EQ(rst.isReady(), true);
}

//! Checks that the parameters of RST are set and initialized as expected
TEST_F(RSTControllerTest, RSTControllerReset)
{
    constexpr size_t controller_length = 7;

    RSTController<controller_length> rst("rst");
    EXPECT_EQ(rst.isReady(), false);

    for (size_t index = 0; index < controller_length - 1; index++)
    {
        EXPECT_EQ(rst.isReady(), false);
        rst.updateInputHistories(index + 1, index);
    }
    // when the history buffers are filled: the RST should be ready
    EXPECT_EQ(rst.isReady(), true);

    rst.reset();
    EXPECT_EQ(rst.isReady(), false);
}


//! Checks that the verification of RST works as expected
TEST_F(RSTControllerTest, RSTControllerVerifyParameters)
{
    constexpr size_t controller_length = 4;

    RSTController<controller_length> rst("rst");

    // set parameters
    std::array<double, controller_length> s_value;
    std::array<double, controller_length> t_value;

    // checks that s(odd) < s(even) is found out by the verification
    s_value                                  = {0.5, 0.6, 0.5, 0.5};
    auto const unstable_s_even_less_than_odd = rst.jurysStabilityTest(s_value);
    ASSERT_EQ(unstable_s_even_less_than_odd.has_value(), true);
    EXPECT_EQ(
        unstable_s_even_less_than_odd.value().warning_str,
        "rst: unstable, sum of even coefficients less or equal than of odd coefficients.\n"
    );

    // checks that t(odd) < t(even) is found out by the verification
    t_value                                  = {0.1, 0.2, 0.0, 0.0};
    auto const unstable_t_even_less_than_odd = rst.jurysStabilityTest(t_value);
    ASSERT_EQ(unstable_t_even_less_than_odd.has_value(), true);
    EXPECT_EQ(
        unstable_t_even_less_than_odd.value().warning_str,
        "rst: unstable, sum of even coefficients less or equal than of odd coefficients.\n"
    );

    // checks that sum of coefficients below 0 is found out by the verification
    t_value                                 = {0.1, 0.2, -1.0, 0.1};
    auto const unstable_coeffs_sum_negative = rst.jurysStabilityTest(t_value);
    ASSERT_EQ(unstable_coeffs_sum_negative.has_value(), true);
    EXPECT_EQ(
        unstable_coeffs_sum_negative.value().warning_str,
        "rst: unstable, sum of even coefficients less or equal than of odd coefficients.\n"
    );

    // checks that roots of coefficients is not above 0 is found out by the verification
    t_value                                   = {0.5, 0.5, 0.5, 0.5};
    auto const unstable_coeffs_roots_negative = rst.jurysStabilityTest(t_value);
    ASSERT_EQ(unstable_coeffs_roots_negative.has_value(), true);
    EXPECT_EQ(
        unstable_coeffs_roots_negative.value().warning_str,
        "rst: unstable, the first element of Jury's array is not above zero.\n"
    );
}

//! Checks that the calculated actuation of RST is as expected
TEST_F(RSTControllerTest, RSTControllerCalculateActuation)
{
    constexpr size_t controller_length = 3;

    RSTController<controller_length> rst("rst");

    // set parameters
    std::array<double, controller_length> r_value = {0.1, 0.2, 0.3};
    std::array<double, controller_length> s_value = {0.5, 0.6, 0.7};
    std::array<double, controller_length> t_value = {0.15, 0.25, 0.35};

    rst.setR(r_value);
    rst.setS(s_value);
    rst.setT(t_value);

    auto maybe_warning = rst.jurysStabilityTest(r_value);
    ASSERT_FALSE(maybe_warning.has_value());
    maybe_warning = rst.jurysStabilityTest(s_value);
    ASSERT_FALSE(maybe_warning.has_value());
    maybe_warning = rst.jurysStabilityTest(t_value);
    ASSERT_FALSE(maybe_warning.has_value());

    double const set_point_value   = 3.14159;
    double const measurement_value = 1.111;

    const double expected_actuation = (t_value[0] * set_point_value - r_value[0] * measurement_value) / s_value[0];
    EXPECT_EQ(rst.control(set_point_value, measurement_value), expected_actuation);

    std::array<double, controller_length> expected_measurement_history = {measurement_value, 0, 0};
    EXPECT_EQ(rst.getMeasurements(), expected_measurement_history);

    std::array<double, controller_length> expected_reference_history = {set_point_value, 0, 0};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);

    std::array<double, controller_length> expected_actuation_history = {expected_actuation, 0, 0};
    EXPECT_EQ(rst.getActuations(), expected_actuation_history);
}

//! Checks that the calculated actuation of RST is as expected
TEST_F(RSTControllerTest, RSTControllerCalculateMultipleActuations)
{
    constexpr size_t controller_length = 3;

    RSTController<controller_length> rst("rst");

    // set parameters
    std::array<double, controller_length> r_value = {0.1, 0.2, 0.3};
    std::array<double, controller_length> s_value = {0.5, 0.6, 0.7};
    std::array<double, controller_length> t_value = {0.15, 0.25, 0.35};

    rst.setR(r_value);
    rst.setS(s_value);
    rst.setT(t_value);

    double const set_point_value   = 3.14159;
    double const measurement_value = 1.111;

    const double first_actuation = (t_value[0] * set_point_value - r_value[0] * measurement_value) / s_value[0];
    EXPECT_NEAR(rst.control(set_point_value, measurement_value), first_actuation, 1e-6);

    // here, the system transfer function is assumed to be unity so next measurement is the previous actuation
    const double second_actuation
        = ((t_value[0] + t_value[1]) * set_point_value - (r_value[0] * first_actuation + r_value[1] * measurement_value)
           - (s_value[1] * first_actuation))
          / s_value[0];
    EXPECT_NEAR(rst.control(set_point_value, first_actuation), second_actuation, 1e-6);

    const double third_actuation
        = ((t_value[0] + t_value[1] + t_value[2]) * set_point_value
           - (r_value[0] * second_actuation + r_value[1] * first_actuation + r_value[2] * measurement_value)
           - (s_value[1] * second_actuation + s_value[2] * first_actuation))
          / s_value[0];
    EXPECT_NEAR(rst.control(set_point_value, second_actuation), third_actuation, 1e-6);

    // history wraps around here
    const double fourth_actuation
        = ((t_value[0] + t_value[1] + t_value[2]) * set_point_value
           - (r_value[0] * third_actuation + r_value[1] * second_actuation + r_value[2] * first_actuation)
           - (s_value[1] * third_actuation + s_value[2] * second_actuation))
          / s_value[0];

    EXPECT_NEAR(rst.control(set_point_value, third_actuation), fourth_actuation, 1e-6);
}

//! Checks that the calculated actuation of RST is as expected
TEST_F(RSTControllerTest, RSTControllerReCalculateReference)
{
    constexpr size_t controller_length = 3;

    RSTController<controller_length> rst("rst");

    // set parameters
    std::array<double, controller_length> r_value = {0.1, 0.2, 0.3};
    std::array<double, controller_length> s_value = {0.5, 0.6, 0.7};
    std::array<double, controller_length> t_value = {0.15, 0.25, 0.35};

    rst.setR(r_value);
    rst.setS(s_value);
    rst.setT(t_value);

    double const set_point_value   = 3.14159;
    double const measurement_value = 1.111;

    const double actuation         = rst.control(set_point_value, measurement_value);
    double const limited_actuation = actuation - 2.0;   // simulates clamping of possible actuations
    rst.updateReference(limited_actuation);

    // measurements should not be modified
    std::array<double, controller_length> expected_measurement_history = {measurement_value, 0, 0};
    EXPECT_EQ(rst.getMeasurements(), expected_measurement_history);

    // actuation should be the clamped actuation
    std::array<double, controller_length> expected_actuation_history = {limited_actuation, 0, 0};
    EXPECT_EQ(rst.getActuations(), expected_actuation_history);

    // reference should be back-calculated
    double const corrected_reference = set_point_value + (limited_actuation - actuation) * s_value[0] / t_value[0];
    std::array<double, controller_length> expected_reference_history = {corrected_reference, 0, 0};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);
}

//! Checks that the calculated actuation of RST is as expected against Simulink model
TEST_F(RSTControllerTest, RSTControllerSimulinkSimpleConsistency)
{
    // simulink model with three filters:
    // 1. Discrete FIR Filter with T0, T1, T2 parameters with rk as input,
    // 2. Discrete FIR FIlter with R0, R1, R2 parameters with yk as input,
    // 3. Subtract outputs from 2. from outputs from 1.
    // 4. Feed the subtraction output to Discrete Filter with S0, S1, S2 parameters, uk is the output
    // Parameter values: Kp = Ki = Kd = Kff = 1, T = 1e-3, N = 2, recalculated to R, S, and T coefficients
    // t has 10000 points, uniformly spaced from 0 to 9999 * T, t cutoff is max of the time
    // yk and rk inputs are randomly generated: rk = randn(10000, 1);

    constexpr size_t                 controller_length = 3;
    RSTController<controller_length> rst("rst");

    // parameters calculated in Matlab
    std::array<double, controller_length> r_value = {3.0015005, -5.999999, 2.9985005};
    std::array<double, controller_length> s_value = {1.001, -2, 0.999};
    std::array<double, controller_length> t_value = {4.0025005, -7.999999, 3.9975005};

    rst.setR(r_value);
    rst.setS(s_value);
    rst.setT(t_value);

    auto maybe_warning = rst.jurysStabilityTest(r_value);
    ASSERT_FALSE(maybe_warning.has_value());
    maybe_warning = rst.jurysStabilityTest(s_value);
    ASSERT_FALSE(maybe_warning.has_value());
    maybe_warning = rst.jurysStabilityTest(t_value);
    ASSERT_FALSE(maybe_warning.has_value());

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path yk_path = "components/inputs/rst_yk_random.csv";
    std::filesystem::path rk_path = "components/inputs/rst_rk_random.csv";
    std::filesystem::path uk_path = "components/inputs/rst_uk_kp=ki=kd=kff=1_N=2_T=1e-3.csv";

    std::ifstream yk_file(yk_path);
    std::ifstream rk_file(rk_path);
    std::ifstream uk_file(uk_path);

    ASSERT_TRUE(yk_file.is_open());
    ASSERT_TRUE(rk_file.is_open());
    ASSERT_TRUE(uk_file.is_open());

    std::string yk_str;
    std::string rk_str;
    std::string uk_str;

    while (getline(yk_file, yk_str) && getline(rk_file, rk_str) && getline(uk_file, uk_str))
    {
        auto const yk_value            = std::stod(yk_str.substr(yk_str.find(",") + 1));
        auto const rk_value            = std::stod(rk_str.substr(rk_str.find(",") + 1));
        auto const matlab_output_value = std::stod(uk_str);   // Matlab output

        auto const actuation = rst.control(rk_value, yk_value);
        auto const relative  = (matlab_output_value - actuation) / matlab_output_value;
        EXPECT_NEAR(relative, 0.0, 1e-6);   // at least 1e-4 relative precision
    }
    yk_file.close();
    rk_file.close();
    uk_file.close();
}

//! Checks that the calculated actuation of RST is as expected against Simulink model
TEST_F(RSTControllerTest, RSTControllerSimulinkConsistency)
{
    // simulink model with three filters:
    // 1. Discrete FIR Filter with T0, T1, T2 parameters with rk as input,
    // 2. Discrete FIR FIlter with R0, R1, R2 parameters with yk as input,
    // 3. Subtract outputs from 2. from outputs from 1.
    // 4. Feed the subtraction output to Discrete Filter with S0, S1, S2 parameters, uk is the output
    // Parameter values: Kp = 52.79, Ki = 0.0472, Kd = 0.0441, Kff = 6.1190, T = 1e-3, N = 17.79,
    // recalculated to R, S, and T coefficients
    // t has 10000 points, uniformly spaced from 0 to 9999 * T, t cutoff is max of the time
    // yk and rk inputs are randomly generated: rk = randn(10000, 1);

    constexpr size_t                 controller_length = 3;
    RSTController<controller_length> rst("rst");

    // parameters calculated in Matlab
    std::array<double, controller_length> r_value = {68.4926325370984, -87.4083295284352, 18.9157413184664};
    std::array<double, controller_length> s_value = {0.51362705, -0.08812, -0.42550705};
    std::array<double, controller_length> t_value = {41.1418629706978, -75.0215524757548, 33.8797338321866};

    rst.setR(r_value);
    rst.setS(s_value);
    rst.setT(t_value);

    auto maybe_warning = rst.jurysStabilityTest(r_value);
    ASSERT_FALSE(maybe_warning.has_value());
    maybe_warning = rst.jurysStabilityTest(s_value);
    ASSERT_FALSE(maybe_warning.has_value());
    maybe_warning = rst.jurysStabilityTest(t_value);
    ASSERT_FALSE(maybe_warning.has_value());

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path yk_path = "components/inputs/rst_yk_random.csv";
    std::filesystem::path rk_path = "components/inputs/rst_rk_random.csv";
    std::filesystem::path uk_path
        = "components/inputs/rst_uk_kp=52p79_ki=0p0472_kd=0p0441_kff=6p1190_N=17p79_T=1e-3.csv";

    std::ifstream yk_file(yk_path);
    std::ifstream rk_file(rk_path);
    std::ifstream uk_file(uk_path);

    ASSERT_TRUE(yk_file.is_open());
    ASSERT_TRUE(rk_file.is_open());
    ASSERT_TRUE(uk_file.is_open());

    std::string yk_str;
    std::string rk_str;
    std::string uk_str;

    while (getline(yk_file, yk_str) && getline(rk_file, rk_str) && getline(uk_file, uk_str))
    {
        auto const yk_value            = std::stod(yk_str.substr(yk_str.find(",") + 1));
        auto const rk_value            = std::stod(rk_str.substr(rk_str.find(",") + 1));
        auto const matlab_output_value = std::stod(uk_str);   // Matlab output

        auto const actuation = rst.control(rk_value, yk_value);
        auto const relative  = (matlab_output_value - actuation) / matlab_output_value;
        EXPECT_NEAR(relative, 0.0, 1e-6);   // at least 1e-4 relative precision
    }
    yk_file.close();
    rk_file.close();
    uk_file.close();
}
