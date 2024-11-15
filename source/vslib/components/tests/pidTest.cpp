//! @file
//! @brief File with unit tests of PID component.
//! @author Dominik Arominski

#include <filesystem>
#include <gtest/gtest.h>

#include "csv.hpp"
#include "pid.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;
using namespace csv;

class PIDTest : public ::testing::Test
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

    void set_pid_parameters(
        PID& pidRst, double p, double i, double d, double ff, double b, double c, double N = 1, double T = 1,
        double f0 = 1, double act_min = 0, double act_max = 1e9
    )
    {
        StaticJson p_value = p;
        pidRst.kp.setJsonValue(p_value);

        StaticJson i_value = i;
        pidRst.ki.setJsonValue(i_value);

        StaticJson d_value = d;
        pidRst.kd.setJsonValue(d_value);

        StaticJson ff_value = ff;
        pidRst.kff.setJsonValue(ff_value);

        StaticJson b_value = b;
        pidRst.b.setJsonValue(b_value);

        StaticJson c_value = c;
        pidRst.c.setJsonValue(c_value);

        StaticJson N_value = N;
        pidRst.N.setJsonValue(N_value);

        StaticJson T_value = T;
        pidRst.T.setJsonValue(T_value);

        StaticJson f0_value = f0;
        pidRst.f0.setJsonValue(f0_value);

        StaticJson act_min_value = act_min;
        pidRst.actuation_limits.min.setJsonValue(act_min_value);

        StaticJson act_max_value = act_max;
        pidRst.actuation_limits.max.setJsonValue(act_max_value);
        pidRst.actuation_limits.verifyParameters();
        pidRst.actuation_limits.flipBufferState();
        pidRst.actuation_limits.synchroniseParameterBuffers();

        pidRst.verifyParameters();
        pidRst.flipBufferState();
        pidRst.synchroniseParameterBuffers();
    }
};


//! Checks that a default PID object can be constructed and is correctly added to the registry
TEST_F(PIDTest, PIDDefaultConstruction)
{
    RootComponent root;
    std::string   name = "pid_1";
    PID           pid(name, root);
    EXPECT_EQ(pid.getName(), name);

    auto serialized_pid = pid.serialize();
    EXPECT_EQ(serialized_pid["name"], name);
    EXPECT_EQ(serialized_pid["type"], "PID");
    EXPECT_EQ(
        serialized_pid["components"].dump(),
        "[{\"name\":\"actuation_limits\",\"type\":\"LimitRange\",\"parameters\":[{\"name\":\"lower_threshold\","
        "\"type\":\"Float64\",\"length\":1,\"value\":{}},{\"name\":\"upper_threshold\",\"type\":\"Float64\",\"length\":"
        "1,\"value\":{}},{\"name\":\"dead_zone\",\"type\":\"ArrayFloat64\",\"length\":2,\"value\":[]}],\"components\":["
        "]}]"
    );
    EXPECT_EQ(serialized_pid["parameters"].size(), 9);
    EXPECT_EQ(serialized_pid["parameters"][0]["name"], "kp");
    EXPECT_EQ(serialized_pid["parameters"][1]["name"], "ki");
    EXPECT_EQ(serialized_pid["parameters"][2]["name"], "kd");
    EXPECT_EQ(serialized_pid["parameters"][3]["name"], "kff");
    EXPECT_EQ(serialized_pid["parameters"][4]["name"], "proportional_scaling");
    EXPECT_EQ(serialized_pid["parameters"][5]["name"], "derivative_scaling");
    EXPECT_EQ(serialized_pid["parameters"][6]["name"], "derivative_filter_order");
    EXPECT_EQ(serialized_pid["parameters"][7]["name"], "control_period");
    EXPECT_EQ(serialized_pid["parameters"][8]["name"], "pre_warping_frequency");
}

//! Checks that the RST coefficients were correctly calculated when kp!=0 or kd!=0
TEST_F(PIDTest, PIDCoefficientsDefault)
{
    RootComponent root;
    std::string   name = "pid_2";
    PID           pid(name, root);
    const double  p  = 2.0;
    const double  i  = 1.0;
    const double  d  = 1.5;
    const double  ff = 0.5;
    const double  b  = 1.0;
    const double  c  = 1.0;
    const double  N  = 1.0;
    const double  T  = 3.0;
    const double  f0 = 2.263752e-6;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0);

    const double a  = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * T);
    const double a2 = a * a;

    std::array<double, 3> expected_r, expected_s, expected_t;

    expected_r[0] = (i * p * N + d * i * a + d * p * a2 + p * p * N * a + d * p * N * a2) / a2;
    expected_r[1] = 2 * (i * p * N - d * p * a2 - d * p * N * a2) / a2;
    expected_r[2] = (i * p * N - d * i * a + d * p * a2 - p * p * N * a + d * p * N * a2) / a2;

    expected_s[0] = (d * a2 + p * N * a) / a2;
    expected_s[1] = -2 * d;
    expected_s[2] = (d * a2 - p * N * a) / a2;

    expected_t[0] = (i * p * N + d * i * a + d * ff * a2 + d * p * a2 * b + p * p * N * a * b + ff * p * N * a
                     + d * p * N * a2 * c)
                    / a2;
    expected_t[1] = 2 * (i * p * N - d * ff * a2 - d * p * a2 * b - d * p * N * a2 * c) / a2;
    expected_t[2] = (i * p * N - d * i * a + d * ff * a2 + d * p * a2 * b - p * p * N * a * b - ff * p * N * a
                     + d * p * N * a2 * c)
                    / a2;

    for (int index = 0; index < 3; index++)
    {
        ASSERT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        ASSERT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        ASSERT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the RST coefficients were correctly calculated when kp=0 and kd != 0
TEST_F(PIDTest, PIDCoefficientsKpZero)
{
    RootComponent root;
    std::string   name = "pid_3";
    PID           pid(name, root);
    const double  p  = 0.0;
    const double  i  = 1.0;
    const double  d  = 1.5;
    const double  ff = 0.1;
    const double  b  = 1.0;
    const double  c  = 1.0;
    const double  N  = 1.0;
    const double  T  = 1e-4;
    const double  f0 = 1e-6;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0);

    const double a  = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * T);
    const double a2 = a * a;

    std::array<double, 3> expected_r, expected_s, expected_t;

    expected_r[0] = (i * p * N + d * i * a + d * p * a2 + p * p * N * a + d * p * N * a2) / a2;
    expected_r[1] = 2 * (i * p * N - d * p * a2 - d * p * N * a2) / a2;
    expected_r[2] = (i * p * N - d * i * a + d * p * a2 - p * p * N * a + d * p * N * a2) / a2;

    expected_s[0] = (d * a2 + p * N * a) / a2;
    expected_s[1] = -2 * d;
    expected_s[2] = (d * a2 - p * N * a) / a2;

    expected_t[0] = (i * p * N + d * i * a + d * ff * a2 + d * p * a2 * b + p * p * N * a * b + ff * p * N * a
                     + d * p * N * a2 * c)
                    / a2;
    expected_t[1] = 2 * (i * p * N - d * ff * a2 - d * p * a2 * b - d * p * N * a2 * c) / a2;
    expected_t[2] = (i * p * N - d * i * a + d * ff * a2 + d * p * a2 * b - p * p * N * a * b - ff * p * N * a
                     + d * p * N * a2 * c)
                    / a2;

    for (int index = 0; index < 3; index++)
    {
        ASSERT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        ASSERT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        ASSERT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the RST coefficients were correctly calculated when kp!=0 and kd=0
TEST_F(PIDTest, PIDCoefficientsKdZero)
{
    RootComponent root;
    std::string   name = "pid_4";
    PID           pid(name, root);
    const double  p  = 3.0;
    const double  i  = 1.0;
    const double  d  = 0.0;
    const double  ff = 0.2;
    const double  b  = 1.0;
    const double  c  = 1.0;
    const double  N  = 1.0;
    const double  T  = 1e-4;
    const double  f0 = 1e-9;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0);

    const double a  = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * T);
    const double a2 = a * a;

    std::array<double, 3> expected_r, expected_s, expected_t;

    expected_r[0] = (i * p * N + d * i * a + d * p * a2 + p * p * N * a + d * p * N * a2) / a2;
    expected_r[1] = 2 * (i * p * N - d * p * a2 - d * p * N * a2) / a2;
    expected_r[2] = (i * p * N - d * i * a + d * p * a2 - p * p * N * a + d * p * N * a2) / a2;

    expected_s[0] = (d * a2 + p * N * a) / a2;
    expected_s[1] = -2 * d;
    expected_s[2] = (d * a2 - p * N * a) / a2;

    expected_t[0] = (i * p * N + d * i * a + d * ff * a2 + d * p * a2 * b + p * p * N * a * b + ff * p * N * a
                     + d * p * N * a2 * c)
                    / a2;
    expected_t[1] = 2 * (i * p * N - d * ff * a2 - d * p * a2 * b - d * p * N * a2 * c) / a2;
    expected_t[2] = (i * p * N - d * i * a + d * ff * a2 + d * p * a2 * b - p * p * N * a * b - ff * p * N * a
                     + d * p * N * a2 * c)
                    / a2;

    for (int index = 0; index < 3; index++)
    {
        ASSERT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        ASSERT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        ASSERT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the RST coefficients were correctly calculated when kd=kff=0, and b=c=1 (1DOF PI)
TEST_F(PIDTest, PIDCoefficientsKdKffZeros)
{
    RootComponent root;
    std::string   name = "pid_5";
    PID           pid(name, root);
    const double  p  = 2.0;
    const double  i  = 1.0;
    const double  d  = 0.0;
    const double  ff = 0.0;
    const double  b  = 1.0;
    const double  c  = 1.0;
    const double  N  = 1.0;
    const double  T  = 1e-4;
    const double  f0 = 1e-9;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0);

    const double a  = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * T);
    const double a2 = a * a;

    std::array<double, 3> expected_r, expected_s, expected_t;

    // simplified equations:
    expected_r[0] = (i * p * N + p * p * a) / a2;
    expected_r[1] = ((i - p * a) / (i + p * a)) * (i * p + p * p * a) / a2;
    expected_r[2] = 0.0;

    expected_s[0] = p / a;
    expected_s[1] = -p / a;
    expected_s[2] = 0.0;

    expected_t[0] = expected_r[0];
    expected_t[1] = expected_r[1];
    expected_t[2] = 0.0;

    for (int index = 0; index < 3; index++)
    {
        ASSERT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        ASSERT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        ASSERT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the RST coefficients were correctly calculated when kp=0 and kd=0
TEST_F(PIDTest, PIDCoefficientsIntegrator)
{
    RootComponent root;
    std::string   name = "pid_6";
    PID           pid(name, root);
    const double  p  = 0.0;
    const double  i  = 1.0;
    const double  d  = 0.0;
    const double  ff = 0.1;
    const double  b  = 1.0;
    const double  c  = 1.0;
    const double  N  = 1.0;
    const double  T  = 3.0;
    const double  f0 = 2.263752e-6;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0);

    const double a = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * T);

    std::array<double, 3> expected_r, expected_s, expected_t;

    expected_r[0] = i / a;
    expected_r[1] = i / a;
    expected_r[2] = 0;

    expected_s[0] = 1;
    expected_s[1] = -1;
    expected_s[2] = 0;

    expected_t[0] = i / a + ff;
    expected_t[1] = i / a - ff;
    expected_t[2] = 0;

    for (int index = 0; index < 3; index++)
    {
        ASSERT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        ASSERT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        ASSERT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the calculated actuation of RST is as expected against Simulink model
TEST_F(PIDTest, PIDSimulinkSimpleConsistency)
{
    RootComponent root;
    // simulink model with three filters:
    // 1. Discrete FIR Filter with T0, T1, T2 parameters with rk as input,
    // 2. Discrete FIR FIlter with R0, R1, R2 parameters with yk as input,
    // 3. Subtract outputs from 2. from outputs from 1.
    // 4. Feed the subtraction output to Discrete Filter with S0, S1, S2 parameters, uk is the output
    // Parameter values: Kp = Ki = Kd = 1, T = 1e-3, N = 2, recalculated to R, S, and T coefficients
    // t has 10000 points, uniformly spaced from 0 to 9999 * T, t cutoff is max of the time
    // yk and rk inputs are randomly generated: rk = randn(10000, 1);

    std::string  name = "pid_7";
    PID          pid(name, root);
    const double p             = 1.0;
    const double i             = 1.0;
    const double d             = 1.0;
    const double ff            = 1.0;
    const double b             = 1.0;
    const double c             = 1.0;
    const double N             = 2.0;
    const double T             = 1.0e-3;
    const double f0            = 1e-15;
    const double actuation_min = -50;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0, actuation_min);

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path yk_path = "components/inputs/rst_yk_random.csv";
    std::filesystem::path rk_path = "components/inputs/rst_rk_random.csv";
    std::filesystem::path uk_path = "components/inputs/rst_uk_kp=ki=kd=kff=1_N=2_T=1e-3.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader yk_file(yk_path.c_str(), format);
    CSVReader rk_file(rk_path.c_str(), format);
    CSVReader uk_file(uk_path.c_str(), format);

    auto yk_line = yk_file.begin();
    auto rk_line = rk_file.begin();
    auto uk_line = uk_file.begin();

    while (yk_line != yk_file.end() && rk_line != rk_file.end() && uk_line != uk_file.end())
    {
        const auto yk_value      = (*yk_line)[1].get<double>();
        const auto rk_value      = (*rk_line)[1].get<double>();
        const auto matlab_output = (*uk_line)[0].get<double>();

        auto const actuation = pid.control(rk_value, yk_value);
        auto const relative  = (matlab_output - actuation) / matlab_output;
        ASSERT_NEAR(relative, 0.0, 1e-6);   // at least 1e-4 relative precision

        ++yk_line;
        ++rk_line;
        ++uk_line;
    }
}

//! Checks that the calculated actuation of RST is as expected against Simulink model
TEST_F(PIDTest, PIDSimulinkConsistency)
{
    RootComponent root;
    // simulink model with three filters:
    // 1. Discrete FIR Filter with T0, T1, T2 parameters with rk as input,
    // 2. Discrete FIR FIlter with R0, R1, R2 parameters with yk as input,
    // 3. Subtract outputs from 2. from outputs from 1.
    // 4. Feed the subtraction output to Discrete Filter with S0, S1, S2 parameters, uk is the output
    // Parameter values: Kp = 52.79, Ki = 0.0472, Kd = 0.0441, Kff = 6.1190, T = 1e-3, N = 17.79,
    // t has 10000 points, uniformly spaced from 0 to 9999 * T, t cutoff is max of the time
    // yk and rk inputs are randomly generated: rk = randn(10000, 1);

    std::string  name = "pid_8";
    PID          pid(name, root);
    const double p             = 52.79;
    const double i             = 0.0472;
    const double d             = 0.04406;
    const double ff            = 6.1190;
    const double b             = 0.03057;
    const double c             = 0.8983;
    const double N             = 17.79;
    const double T             = 1.0e-3;
    const double f0            = 1e-15;
    const double actuation_min = -1e13;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0, actuation_min);

    // the input files are randomly generated numbers
    std::filesystem::path yk_path = "components/inputs/rst_yk_random.csv";
    std::filesystem::path rk_path = "components/inputs/rst_rk_random.csv";
    std::filesystem::path uk_path
        = "components/inputs/rst_uk_kp=52p79_ki=0p0472_kd=0p0441_kff=6p1190_N=17p79_T=1e-3.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader yk_file(yk_path.c_str(), format);
    CSVReader rk_file(rk_path.c_str(), format);
    CSVReader uk_file(uk_path.c_str(), format);

    auto yk_line = yk_file.begin();
    auto rk_line = rk_file.begin();
    auto uk_line = uk_file.begin();

    while (yk_line != yk_file.end() && rk_line != rk_file.end() && uk_line != uk_file.end())
    {
        const auto yk_value      = (*yk_line)[1].get<double>();
        const auto rk_value      = (*rk_line)[1].get<double>();
        const auto matlab_output = (*uk_line)[0].get<double>();

        auto const actuation = pid.control(rk_value, yk_value);
        auto const relative  = (matlab_output - actuation) / matlab_output;
        ASSERT_NEAR(relative, 0.0, 1e-6);   // at least 1e-4 relative precision

        ++yk_line;
        ++rk_line;
        ++uk_line;
    }
}

//! Checks that the calculated actuation of RST is as expected against Simulink model
TEST_F(PIDTest, PIDSimulinkIntegratorConsistency)
{
    RootComponent root;
    // simulink model with three filters:
    // 1. Discrete FIR Filter with T0, T1, T2 parameters with rk as input,
    // 2. Discrete FIR FIlter with R0, R1, R2 parameters with yk as input,
    // 3. Subtract outputs from 2. from outputs from 1.
    // 4. Feed the subtraction output to Discrete Filter with S0, S1, S2 parameters, uk is the output
    // Parameter values: Kp = Kd = 0, Ki = 0.0472, Kff = 6.1190, T = 1e-3, N = 17.79,
    // t has 10000 points, uniformly spaced from 0 to 9999 * T, t cutoff is max of the time
    // yk and rk inputs are randomly generated: rk = randn(10000, 1);

    std::string  name = "pid_9";
    PID          pid(name, root);
    const double p             = 0;
    const double i             = 0.0472;
    const double d             = 0;
    const double ff            = 6.1190;
    const double b             = 0.03057;
    const double c             = 0.8983;
    const double N             = 17.79;
    const double T             = 1.0e-3;
    const double f0            = 1e-15;
    const double actuation_min = -50;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0, actuation_min);

    // fill the histories to enable the controller:
    EXPECT_EQ(pid.control(0, 0), 0);
    EXPECT_EQ(pid.control(0, 0), 0);
    EXPECT_EQ(pid.control(0, 0), 0);
    // now, the controller is enabled and actuations can be calculated

    // the input files are randomly generated numbers
    std::filesystem::path yk_path = "components/inputs/rst_yk_random.csv";
    std::filesystem::path rk_path = "components/inputs/rst_rk_random.csv";
    std::filesystem::path uk_path = "components/inputs/rst_uk_kp=kd=0_ki=0p0472_kff=6p1190_N=17p79_T=1e-3.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader yk_file(yk_path.c_str(), format);
    CSVReader rk_file(rk_path.c_str(), format);
    CSVReader uk_file(uk_path.c_str(), format);

    auto yk_line = yk_file.begin();
    auto rk_line = rk_file.begin();
    auto uk_line = uk_file.begin();

    while (yk_line != yk_file.end() && rk_line != rk_file.end() && uk_line != uk_file.end())
    {
        const auto yk_value      = (*yk_line)[1].get<double>();
        const auto rk_value      = (*rk_line)[1].get<double>();
        const auto matlab_output = (*uk_line)[0].get<double>();

        auto const actuation = pid.control(rk_value, yk_value);
        auto const relative  = (matlab_output - actuation) / matlab_output;
        ASSERT_NEAR(relative, 0.0, 1e-6);   // at least 1e-4 relative precision

        ++yk_line;
        ++rk_line;
        ++uk_line;
    }
}

//! Checks the consistency of PI controller behaviour vs a PI model written in Simulink,
//! as part of a Phase-Locked Loop.
TEST_F(PIDTest, PIDSimulinkPIinPLL)
{
    RootComponent root;

    std::string  name = "pi_10";
    PID          pid(name, root);
    const double p             = 50;
    const double i             = 200;
    const double d             = 0;
    const double ff            = 0;
    const double b             = 1.0;
    const double c             = 1.0;
    const double N             = 1.0;
    const double T             = 1.0e-4;
    const double f0            = 1e-15;
    const double actuation_min = -1e9;
    const double actuation_max = 1e9;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, T, f0, actuation_min, actuation_max);

    // the input files are randomly generated numbers
    std::filesystem::path pid_meas_path      = "components/inputs/pll_pi_meas.csv";
    std::filesystem::path pid_actuation_path = "components/inputs/pll_act_pi_kp=50_ki=200.csv";

    csv::CSVFormat format;
    format.header_row(-1);   // Disables header handling

    CSVReader pid_meas_file(pid_meas_path.c_str(), format);
    CSVReader pid_act_file(pid_actuation_path.c_str(), format);

    auto meas_line = pid_meas_file.begin();
    auto act_line  = pid_act_file.begin();

    while (meas_line != pid_meas_file.end() && act_line != pid_act_file.end())
    {
        const auto meas_value     = (*meas_line)[0].get<double>();
        const auto pid_act_matlab = (*act_line)[0].get<double>();

        auto const actuation = pid.control(0.0, -meas_value);
        double     relative;
        if (pid_act_matlab != 0)
        {
            relative = (pid_act_matlab - actuation) / pid_act_matlab;
        }
        else
        {
            relative = (pid_act_matlab - actuation);
        }
        ASSERT_NEAR(relative, 0.0, 1e-6);   // at least 1e-6 relative precision

        ++meas_line;
        ++act_line;
    }
}
