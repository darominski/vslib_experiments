//! @file
//! @brief File with unit tests of PIDRST component.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "pidRst.h"
#include "staticJson.h"

using namespace vslib;

class PIDRSTTest : public ::testing::Test
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
        PIDRST& pidRst, double p, double i, double d, double ff, double b, double c, size_t N = 1, double ts = 1,
        double f0 = 1
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

        StaticJson ts_value = ts;
        pidRst.ts.setJsonValue(ts_value);

        StaticJson f0_value = f0;
        pidRst.f0.setJsonValue(f0_value);

        pidRst.verifyParameters();
        pidRst.flipBufferState();
        pidRst.synchroniseParameterBuffers();
    }
};


//! Checks that a default PID object can be constructed and is correctly added to the registry
TEST_F(PIDRSTTest, PIDRSTDefaultConstruction)
{
    std::string name = "pid_1";
    PIDRST      pid(name);
    EXPECT_EQ(pid.getName(), name);

    auto serialized_pid = pid.serialize();
    EXPECT_EQ(serialized_pid["name"], name);
    EXPECT_EQ(serialized_pid["type"], "PID");
    EXPECT_EQ(serialized_pid["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_pid["parameters"].size(), 9);
    EXPECT_EQ(serialized_pid["parameters"][0]["name"], "kp");
    EXPECT_EQ(serialized_pid["parameters"][1]["name"], "ki");
    EXPECT_EQ(serialized_pid["parameters"][2]["name"], "kd");
    EXPECT_EQ(serialized_pid["parameters"][3]["name"], "kff");
    EXPECT_EQ(serialized_pid["parameters"][4]["name"], "proportional_scaling");
    EXPECT_EQ(serialized_pid["parameters"][5]["name"], "derivative_scaling");
    EXPECT_EQ(serialized_pid["parameters"][6]["name"], "derivative_filter_order");
    EXPECT_EQ(serialized_pid["parameters"][7]["name"], "sampling_period");
    EXPECT_EQ(serialized_pid["parameters"][8]["name"], "control_frequency");
}

//! Checks that the RST coefficients were correctly calculated when kp!=0 or kd!=0
TEST_F(PIDRSTTest, PIDRSTCoefficientsDefault)
{
    std::string  name = "pid_2";
    PIDRST       pid(name);
    const double p  = 2.0;
    const double i  = 1.0;
    const double d  = 1.5;
    const double ff = 0.5;
    const double b  = 1.0;
    const double c  = 1.0;
    const double N  = 1.0;
    const double ts = 3.0;
    const double f0 = 2.263752e-6;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, ts, f0);

    const double a  = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * ts);
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
        EXPECT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        EXPECT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        EXPECT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the RST coefficients were correctly calculated when kp=0 and kd!=0
TEST_F(PIDRSTTest, PIDRSTCoefficientsKpZero)
{
    std::string  name = "pid_2";
    PIDRST       pid(name);
    const double p  = 0.0;
    const double i  = 1.0;
    const double d  = 1.5;
    const double ff = 0.1;
    const double b  = 1.0;
    const double c  = 1.0;
    const double N  = 1.0;
    const double ts = 3.0;
    const double f0 = 2.263752e-6;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, ts, f0);

    const double a  = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * ts);
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
        EXPECT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        EXPECT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        EXPECT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the RST coefficients were correctly calculated when kp!=0 and kd=0
TEST_F(PIDRSTTest, PIDRSTCoefficientsKdZero)
{
    std::string  name = "pid_2";
    PIDRST       pid(name);
    const double p  = 2.0;
    const double i  = 1.0;
    const double d  = 0.0;
    const double ff = 0.2;
    const double b  = 1.0;
    const double c  = 1.0;
    const double N  = 1.0;
    const double ts = 3.0;
    const double f0 = 2.263752e-6;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, ts, f0);

    const double a  = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * ts);
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
        EXPECT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        EXPECT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        EXPECT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the RST coefficients were correctly calculated when kp=0 and kd=0
TEST_F(PIDRSTTest, PIDRSTCoefficientsIntegrator)
{
    std::string  name = "pid_2";
    PIDRST       pid(name);
    const double p  = 0.0;
    const double i  = 1.0;
    const double d  = 0.0;
    const double ff = 0.1;
    const double b  = 1.0;
    const double c  = 1.0;
    const double N  = 1.0;
    const double ts = 3.0;
    const double f0 = 2.263752e-6;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, ts, f0);

    const double a = 2.0 * std::numbers::pi_v<double> * f0 / tan(std::numbers::pi_v<double> * f0 * ts);

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
        EXPECT_NEAR(pid.getR()[index], expected_r[index], 1e-12);
        EXPECT_NEAR(pid.getS()[index], expected_s[index], 1e-12);
        EXPECT_NEAR(pid.getT()[index], expected_t[index], 1e-12);
    }
}

//! Checks that the calculated actuation of RST is as expected against Simulink model
TEST_F(PIDRSTTest, PIDRSTSimulinkConsistency)
{
    // simulink model with three filters:
    // 1. Discrete FIR Filter with T0, T1, T2 parameters with rk as input,
    // 2. Discrete FIR FIlter with R0, R1, R2 parameters with yk as input,
    // 3. Subtract outputs from 2. from outputs from 1.
    // 4. Feed the subtraction output to Discrete Filter with S0, S1, S2 parameters, uk is the output
    // Parameter values: Kp = Ki = Kd = 1, T = 1e-3, N = 2, recalculated to R, S, and T coefficients
    // t has 10000 points, uniformly spaced from 0 to 9999 * T, t cutoff is max of the time
    // yk and rk inputs are randomly generated: rk = randn(10000, 1);

    std::string  name = "pid";
    PIDRST       pid(name);
    const double p  = 1.0;
    const double i  = 1.0;
    const double d  = 1.0;
    const double ff = 1.0;
    const double b  = 1.0;
    const double c  = 1.0;
    const double N  = 2.0;
    const double ts = 1.0e-3;
    const double f0 = 1e-15;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, ts, f0);

    std::cout << pid.getR()[0] << " " << pid.getR()[1] << " " << pid.getR()[2] << std::endl;
    std::cout << pid.getS()[0] << " " << pid.getS()[1] << " " << pid.getS()[2] << std::endl;
    std::cout << pid.getT()[0] << " " << pid.getT()[1] << " " << pid.getT()[2] << std::endl;

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path yk_path = "components/inputs/rst_yk_kp=ki=kd=kff=1_N=2_T=1e-3.csv";
    std::filesystem::path rk_path = "components/inputs/rst_rk_kp=ki=kd=kff_1_N=2_T=1e-3.csv";
    std::filesystem::path uk_path = "components/inputs/rst_uk_kp=ki=kd=kff_1_N=2_T=1e-3.csv";

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

        auto const actuation = pid.control(yk_value, rk_value);
        auto const relative  = (matlab_output_value - actuation) / matlab_output_value;

        EXPECT_NEAR(relative, 0.0, 1e-6);   // at least 1e-6 relative precision
    }
    yk_file.close();
    rk_file.close();
    uk_file.close();
}