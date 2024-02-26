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
        pidRst.kp.synchroniseWriteBuffer();

        StaticJson i_value = i;
        pidRst.ki.setJsonValue(i_value);
        pidRst.ki.synchroniseWriteBuffer();

        StaticJson d_value = d;
        pidRst.kd.setJsonValue(d_value);
        pidRst.kd.synchroniseWriteBuffer();

        StaticJson ff_value = ff;
        pidRst.kff.setJsonValue(ff_value);
        pidRst.kff.synchroniseWriteBuffer();

        StaticJson b_value = b;
        pidRst.b.setJsonValue(b_value);
        pidRst.b.synchroniseWriteBuffer();

        StaticJson c_value = c;
        pidRst.c.setJsonValue(c_value);
        pidRst.c.synchroniseWriteBuffer();

        StaticJson N_value = N;
        pidRst.N.setJsonValue(N_value);
        pidRst.N.synchroniseWriteBuffer();

        StaticJson ts_value = ts;
        pidRst.ts.setJsonValue(ts_value);
        pidRst.ts.synchroniseWriteBuffer();

        StaticJson f0_value = f0;
        pidRst.f0.setJsonValue(f0_value);
        pidRst.f0.synchroniseWriteBuffer();

        BufferSwitch::flipState();
        pidRst.verifyParameters();
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
