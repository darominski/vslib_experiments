//! @file
//! @brief File with unit tests of PIDRST component.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "componentRegistry.h"
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
        ComponentRegistry& component_registry = ComponentRegistry::instance();
        component_registry.clearRegistry();
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

        pidRst.flipBufferState();
        pidRst.verifyParameters();
        pidRst.f0.syncInactiveBuffer();
    }
};


//! Checks that a default PID object can be constructed and is correctly added to the registry
TEST_F(PIDRSTTest, PIDRSTDefaultConstruction)
{
    std::string name = "pid_1";
    PIDRST      pid(name);
    EXPECT_EQ(pid.getName(), name);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(pid.getFullName()), registry.getComponents().end());

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

//! Checks that single iteration of control method correctly calculates the gain
TEST_F(PIDRSTTest, PIDRSTSingleIteration)
{
    std::string  name = "pid_2";
    PIDRST       pid(name);
    double const p  = 2.0;
    double const i  = 1.0;
    double const d  = 1.5;
    double const ff = 0.0;
    double const b  = 1.0;
    double const c  = 1.0;
    double const N  = 1.0;
    double const ts = 3.0;
    double const f0 = 2.263752;
    set_pid_parameters(pid, p, i, d, ff, b, c, N, ts, f0);

    const double target_value = 3.14159;

    const double starting_value = 1.0;

    const double error          = target_value - starting_value;
    const double expected_value = (target_value * b - starting_value) * p + error * i
                                  + d * (1 / (1 + d / (N * p))) * (target_value * c - starting_value)
                                  + starting_value * ff;

    EXPECT_NEAR(pid.control(starting_value, target_value), expected_value, 1e-6);
}
