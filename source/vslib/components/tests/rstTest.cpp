//! @file
//! @brief File with unit tests of RST component.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "rst.h"
#include "staticJson.h"

using namespace vslib;

class RSTTest : public ::testing::Test
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

    template<size_t length>
    void set_rst_parameters(
        RST<length>& rst, std::array<double, length>& r, std::array<double, length>& s, std::array<double, length>& t,
        double act_min = 0, double act_max = 1e9
    )
    {
        StaticJson r_value = r;
        rst.r.setJsonValue(r_value);

        StaticJson s_value = s;
        rst.s.setJsonValue(s_value);

        StaticJson t_value = t;
        rst.t.setJsonValue(t_value);

        StaticJson act_min_value = act_min;
        rst.actuation_limits.min.setJsonValue(act_min_value);

        StaticJson act_max_value = act_max;
        rst.actuation_limits.max.setJsonValue(act_max_value);
        rst.actuation_limits.verifyParameters();
        rst.actuation_limits.flipBufferState();
        rst.actuation_limits.synchroniseParameterBuffers();

        rst.verifyParameters();
        rst.flipBufferState();
        rst.synchroniseParameterBuffers();
    }
};

//! Checks that a default RST object can be constructed and is correctly added to the registry
TEST_F(RSTTest, RSTDefaultConstruction)
{
    std::string      name              = "rst_1";
    constexpr size_t controller_length = 3;

    RST<controller_length> rst(name, nullptr);

    EXPECT_EQ(rst.getName(), name);
    EXPECT_EQ(rst.r.isInitialized(), false);
    EXPECT_EQ(rst.s.isInitialized(), false);
    EXPECT_EQ(rst.t.isInitialized(), false);
    EXPECT_EQ(rst.isReady(), false);

    auto serialized = rst.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "RST");
    EXPECT_EQ(
        serialized["components"].dump(),
        "[{\"name\":\"actuation_limits\",\"type\":\"LimitRange\",\"parameters\":[{\"name\":\"lower_threshold\","
        "\"type\":\"Float64\",\"length\":1,\"value\":{}},{\"name\":\"upper_threshold\",\"type\":\"Float64\",\"length\":"
        "1,\"value\":{}},{\"name\":\"dead_zone\",\"type\":\"ArrayFloat64\",\"length\":2,\"value\":[]}],\"components\":["
        "]}]"
    );
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "r");
    EXPECT_EQ(serialized["parameters"][0]["length"], controller_length);

    EXPECT_EQ(serialized["parameters"][1]["name"], "s");
    EXPECT_EQ(serialized["parameters"][1]["length"], controller_length);

    EXPECT_EQ(serialized["parameters"][2]["name"], "t");
    EXPECT_EQ(serialized["parameters"][2]["length"], controller_length);
}

//! Checks that the input histories can be updated and when enough of points are provided, the RST is ready to control
TEST_F(RSTTest, RSTUpdateInputHistories)
{
    std::string      name              = "rst_2";
    constexpr size_t controller_length = 5;

    RST<controller_length> rst(name, nullptr);

    for (size_t index = 0; index < controller_length; index++)
    {
        EXPECT_EQ(rst.isReady(), false);
        rst.updateInputHistories(index, index + 1);
    }
    // when the history buffers are filled: the RST should be ready
    EXPECT_EQ(rst.isReady(), true);
}

//! Checks that the parameters of RST are set and initialized as expected
TEST_F(RSTTest, RSTReset)
{
    std::string      name              = "rst_3";
    constexpr size_t controller_length = 7;

    RST<controller_length> rst(name, nullptr);
    EXPECT_EQ(rst.isReady(), false);

    for (size_t index = 0; index < controller_length; index++)
    {
        EXPECT_EQ(rst.isReady(), false);
        rst.updateInputHistories(index, index + 1);
    }
    // when the history buffers are filled: the RST should be ready
    EXPECT_EQ(rst.isReady(), true);

    rst.reset();
    EXPECT_EQ(rst.isReady(), false);
}

//! Checks that the parameters of RST can be set and initialized as expected
TEST_F(RSTTest, RSTSetParameters)
{
    std::string      name              = "rst_3";
    constexpr size_t controller_length = 4;

    RST<controller_length> rst(name, nullptr);

    // ensure the initial state is as expected:
    EXPECT_EQ(rst.r.isInitialized(), false);
    EXPECT_EQ(rst.s.isInitialized(), false);
    EXPECT_EQ(rst.t.isInitialized(), false);
    for (size_t index = 0; index < controller_length; index++)
    {
        EXPECT_EQ(rst.r[index], 0.0);
        EXPECT_EQ(rst.s[index], 0.0);
        EXPECT_EQ(rst.t[index], 0.0);
    }

    // set parameters
    std::array<double, controller_length> r_value = {0.1, 0.2, 0.3, 0.4};
    std::array<double, controller_length> s_value = {0.5, 0.6, 0.7, 0.8};
    std::array<double, controller_length> t_value = {0.15, 0.25, 0.35, 0.45};

    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);

    EXPECT_EQ(rst.r.isInitialized(), true);
    EXPECT_EQ(rst.s.isInitialized(), true);
    EXPECT_EQ(rst.t.isInitialized(), true);
    for (size_t index = 0; index < controller_length; index++)
    {
        EXPECT_EQ(rst.r[index], r_value[index]);
        EXPECT_EQ(rst.s[index], s_value[index]);
        EXPECT_EQ(rst.t[index], t_value[index]);
    }
}

//! Checks that the verification of RST works as expected
TEST_F(RSTTest, RSTVerifyParameters)
{
    std::string      name              = "rst_3";
    constexpr size_t controller_length = 4;

    RST<controller_length> rst(name, nullptr);

    // set parameters
    std::array<double, controller_length> r_value = {0.0, 0.2, 0.3, 0.4};
    std::array<double, controller_length> s_value = {0.0, 0.6, 0.7, 0.8};
    std::array<double, controller_length> t_value = {0.0, 0.25, 0.35, 0.45};

    // checks that r[0]=0 is found out by the verification
    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    auto const r_does_not_pass = rst.verifyParameters();
    ASSERT_EQ(r_does_not_pass.has_value(), true);
    EXPECT_EQ(r_does_not_pass.value().warning_str, "First element of r coefficients is zero.\n");

    // checks that s[0]=0 is found out by the verification
    r_value = {0.1, 0.2, 0.3, 0.4};
    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    auto const s_does_not_pass = rst.verifyParameters();
    ASSERT_EQ(s_does_not_pass.has_value(), true);
    EXPECT_EQ(s_does_not_pass.value().warning_str, "First element of s coefficients is zero.\n");

    // checks that t[0]=0 is found out by the verification
    s_value = {0.5, 0.6, 0.7, 0.8};
    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    auto const t_does_not_pass = rst.verifyParameters();
    ASSERT_EQ(t_does_not_pass.has_value(), true);
    EXPECT_EQ(t_does_not_pass.value().warning_str, "First element of t coefficients is zero.\n");


    // checks that s(odd) < s(even) is found out by the verification
    s_value = {0.5, 0.6, 0.5, 0.5};
    t_value = {0.1, 0.1, 0.1, 0.1};
    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    auto const unstable_s_even_less_than_odd = rst.verifyParameters();
    ASSERT_EQ(unstable_s_even_less_than_odd.has_value(), true);
    EXPECT_EQ(
        unstable_s_even_less_than_odd.value().warning_str,
        "RST unstable: sum of even coefficients less or equal than of odd coefficients.\n"
    );

    // checks that t(odd) < t(even) is found out by the verification
    s_value = {0.5, 0.5, 0.4, 0.3};
    t_value = {0.1, 0.2, 0.0, 0.0};
    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    auto const unstable_t_even_less_than_odd = rst.verifyParameters();
    ASSERT_EQ(unstable_t_even_less_than_odd.has_value(), true);
    EXPECT_EQ(
        unstable_t_even_less_than_odd.value().warning_str,
        "RST unstable: sum of even coefficients less or equal than of odd coefficients.\n"
    );

    // checks that sum of coefficients below 0 is found out by the verification
    s_value = {0.5, 0.5, 0.4, 0.3};
    t_value = {0.1, 0.2, -1.0, 0.1};
    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    auto const unstable_coeffs_sum_negative = rst.verifyParameters();
    ASSERT_EQ(unstable_coeffs_sum_negative.has_value(), true);
    EXPECT_EQ(
        unstable_coeffs_sum_negative.value().warning_str,
        "RST unstable: sum of even coefficients less or equal than of odd coefficients.\n"
    );

    // checks that roots of coefficients is not above 0 is found out by the verification
    s_value = {0.5, 0.5, 0.5, 0.5};
    t_value = {0.5, 0.5, 0.5, 0.5};
    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    auto const unstable_coeffs_roots_negative = rst.verifyParameters();
    ASSERT_EQ(unstable_coeffs_roots_negative.has_value(), true);
    EXPECT_EQ(
        unstable_coeffs_roots_negative.value().warning_str,
        "RST unstable: the first element of Jury's array is not above zero.\n"
    );
}

//! Checks that the calculated actuation of RST is as expected
TEST_F(RSTTest, RSTCalculateActuation)
{
    std::string      name              = "rst_3";
    constexpr size_t controller_length = 3;

    RST<controller_length> rst(name, nullptr);

    // set parameters
    std::array<double, controller_length> r_value = {0.1, 0.2, 0.3};
    std::array<double, controller_length> s_value = {0.5, 0.6, 0.7};
    std::array<double, controller_length> t_value = {0.15, 0.25, 0.35};

    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    const auto maybe_warning = rst.verifyParameters();
    ASSERT_FALSE(maybe_warning.has_value());

    // fill the histories to enable RST:
    EXPECT_EQ(rst.control(0, 0), 0);
    EXPECT_EQ(rst.control(0, 0), 0);
    EXPECT_EQ(rst.control(0, 0), 0);
    // now the RST is enabled and actuation can be calculated

    double const set_point_value   = 3.14159;
    double const measurement_value = 1.111;

    const double expected_actuation = (t_value[0] * set_point_value - r_value[0] * measurement_value) / s_value[0];
    EXPECT_EQ(rst.control(measurement_value, set_point_value), expected_actuation);

    std::array<double, controller_length> expected_measurement_history = {measurement_value, 0, 0};
    EXPECT_EQ(rst.getMeasurements(), expected_measurement_history);

    std::array<double, controller_length> expected_reference_history = {set_point_value, 0, 0};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);

    std::array<double, controller_length> expected_actuation_history = {expected_actuation, 0, 0};
    EXPECT_EQ(rst.getActuations(), expected_actuation_history);
}

//! Checks that the calculated actuation of RST is as expected
TEST_F(RSTTest, RSTCalculateMultipleActuations)
{
    std::string      name              = "rst_3";
    constexpr size_t controller_length = 3;

    RST<controller_length> rst(name, nullptr);

    // set parameters
    std::array<double, controller_length> r_value = {0.1, 0.2, 0.3};
    std::array<double, controller_length> s_value = {0.5, 0.6, 0.7};
    std::array<double, controller_length> t_value = {0.15, 0.25, 0.35};

    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    const auto maybe_warning = rst.verifyParameters();
    ASSERT_FALSE(maybe_warning.has_value());

    // fill the histories to enable RST:
    EXPECT_EQ(rst.control(0, 0), 0);
    EXPECT_EQ(rst.control(0, 0), 0);
    EXPECT_EQ(rst.control(0, 0), 0);
    // now the RST is enabled and actuation can be calculated

    double const set_point_value   = 3.14159;
    double const measurement_value = 1.111;

    const double first_actuation = (t_value[0] * set_point_value - r_value[0] * measurement_value) / s_value[0];
    EXPECT_NEAR(rst.control(measurement_value, set_point_value), first_actuation, 1e-6);

    // here, the system transfer function is assumed to be unity so next measurement is the previous actuation
    const double second_actuation
        = ((t_value[0] + t_value[1]) * set_point_value - (r_value[0] * first_actuation + r_value[1] * measurement_value)
           - (s_value[1] * first_actuation))
          / s_value[0];
    EXPECT_NEAR(rst.control(first_actuation, set_point_value), second_actuation, 1e-6);

    const double third_actuation
        = ((t_value[0] + t_value[1] + t_value[2]) * set_point_value
           - (r_value[0] * second_actuation + r_value[1] * first_actuation + r_value[2] * measurement_value)
           - (s_value[1] * second_actuation + s_value[2] * first_actuation))
          / s_value[0];
    EXPECT_NEAR(rst.control(second_actuation, set_point_value), third_actuation, 1e-6);

    // history wraps around here
    const double fourth_actuation
        = ((t_value[0] + t_value[1] + t_value[2]) * set_point_value
           - (r_value[0] * third_actuation + r_value[1] * second_actuation + r_value[2] * first_actuation)
           - (s_value[1] * third_actuation + s_value[2] * second_actuation))
          / s_value[0];

    EXPECT_NEAR(rst.control(third_actuation, set_point_value), fourth_actuation, 1e-6);
}

//! Checks that the calculated actuation of RST is as expected
TEST_F(RSTTest, RSTReCalculateReference)
{
    std::string      name              = "rst_3";
    constexpr size_t controller_length = 3;

    RST<controller_length> rst(name, nullptr);

    // set parameters
    std::array<double, controller_length> r_value = {0.1, 0.2, 0.3};
    std::array<double, controller_length> s_value = {0.5, 0.6, 0.7};
    std::array<double, controller_length> t_value = {0.15, 0.25, 0.35};

    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value);
    const auto maybe_warning = rst.verifyParameters();
    ASSERT_FALSE(maybe_warning.has_value());

    double const set_point_value   = 3.14159;
    double const measurement_value = 1.111;

    const double actuation         = rst.control(measurement_value, set_point_value);
    double const limited_actuation = actuation - 2.0;   // simulates clamping of possible actuations
    rst.updateReference(limited_actuation);

    // measurements should not be modified
    std::array<double, controller_length> expected_measurement_history = {measurement_value, 0, 0};
    EXPECT_EQ(rst.getMeasurements(), expected_measurement_history);

    // actuation should be the clamped actuation
    std::array<double, controller_length> expected_actuation_history = {limited_actuation, 0, 0};
    EXPECT_EQ(rst.getActuations(), expected_actuation_history);

    // reference should be back-calculated
    double const corrected_reference = s_value[0] * limited_actuation + r_value[0] * measurement_value;
    std::array<double, controller_length> expected_reference_history = {corrected_reference, 0, 0};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);
}

//! Checks that the calculated actuation of RST is as expected even if the initially calculated actuation violates the
//! defined max limit
TEST_F(RSTTest, RSTLimitedActuation)
{
    std::string      name              = "rst_3";
    constexpr size_t controller_length = 3;

    RST<controller_length> rst(name, nullptr);

    // set parameters
    std::array<double, controller_length> r_value       = {0.1, 0.2, 0.3};
    std::array<double, controller_length> s_value       = {0.5, 0.6, 0.7};
    std::array<double, controller_length> t_value       = {0.15, 0.25, 0.35};
    const double                          min_actuation = -1;
    const double                          max_actuation = 10;

    set_rst_parameters<controller_length>(rst, r_value, s_value, t_value, min_actuation, max_actuation);
    const auto maybe_warning = rst.verifyParameters();
    ASSERT_FALSE(maybe_warning.has_value());

    // fill the histories to enable RST:
    EXPECT_EQ(rst.control(0, 0), 0);
    EXPECT_EQ(rst.control(0, 0), 0);
    EXPECT_EQ(rst.control(0, 0), 0);

    // first iteration
    const double set_point_value   = 100;
    const double measurement_value = 0.5;

    double unlimited_calculation = (t_value[0] * set_point_value - r_value[0] * measurement_value) / s_value[0];
    double calculated_actuation  = rst.control(measurement_value, set_point_value);
    EXPECT_NE(calculated_actuation, unlimited_calculation);
    EXPECT_NEAR(calculated_actuation, max_actuation, 1e-6);

    const double corrected_reference = s_value[0] * calculated_actuation + r_value[0] * measurement_value;
    std::array<double, controller_length> expected_reference_history = {corrected_reference, 0, 0};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);

    // second iteration
    const double second_unlimited_calculation
        = ((t_value[0] + t_value[1]) * set_point_value
           - (r_value[0] * calculated_actuation + r_value[1] * measurement_value) - (s_value[1] * calculated_actuation))
          / s_value[0];
    const double second_calculated_actuation = rst.control(measurement_value, set_point_value);
    EXPECT_NE(second_calculated_actuation, second_unlimited_calculation);
    EXPECT_NEAR(second_calculated_actuation, max_actuation, 1e-6);

    const double second_corrected_reference = s_value[0] * second_calculated_actuation + r_value[0] * measurement_value
                                              + s_value[1] * calculated_actuation + r_value[1] * measurement_value
                                              - t_value[1] * corrected_reference;

    expected_reference_history = {second_corrected_reference, corrected_reference, 0};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);

    // third iteration
    const double third_calculated_actuation = rst.control(measurement_value, set_point_value);
    EXPECT_NEAR(third_calculated_actuation, max_actuation, 1e-6);

    const double third_corrected_reference = s_value[0] * third_calculated_actuation + r_value[0] * measurement_value
                                             + s_value[1] * second_calculated_actuation + r_value[1] * measurement_value
                                             - t_value[1] * second_corrected_reference
                                             + s_value[2] * calculated_actuation + r_value[2] * measurement_value
                                             - t_value[2] * corrected_reference;

    expected_reference_history = {third_corrected_reference, second_corrected_reference, corrected_reference};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);

    // fourth iteration, history wraps around here
    const double fourth_calculated_actuation = rst.control(measurement_value, set_point_value);
    EXPECT_NEAR(fourth_calculated_actuation, max_actuation, 1e-6);

    const double fourth_corrected_reference
        = s_value[0] * fourth_calculated_actuation + r_value[0] * measurement_value
          + s_value[1] * third_calculated_actuation + r_value[1] * measurement_value
          - t_value[1] * third_corrected_reference + s_value[2] * second_calculated_actuation
          + r_value[2] * measurement_value - t_value[2] * second_corrected_reference;

    expected_reference_history = {fourth_corrected_reference, third_corrected_reference, second_corrected_reference};
    EXPECT_EQ(rst.getReferences(), expected_reference_history);
}