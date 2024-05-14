//! @file
//! @brief File with unit tests of PIDRST component.
//! @author Dominik Arominski

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
