//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "limitRms.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;

class LimitRmsTest : public ::testing::Test
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

    void set_limit_parameters(LimitRms& limit, double rms, double rms_time_constant)
    {
        StaticJson rms_val = rms;
        limit.rms_limit.setJsonValue(rms_val);

        StaticJson rms_time_constant_val = rms_time_constant;
        limit.rms_time_constant.setJsonValue(rms_time_constant_val);

        limit.verifyParameters();
        limit.flipBufferState();
        limit.synchroniseParameterBuffers();
    }
};

//! Tests default construction of integral type Limit component
TEST_F(LimitRmsTest, LimitRmsDefault)
{
    RootComponent root;
    std::string   name = "limit";
    LimitRms      limit(name, root);
    EXPECT_EQ(limit.getName(), name);

    auto serialized = limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRms");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "rms_limit");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][1]["name"], "rms_time_constant");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float64");
}

//! Tests catching value with excessive RMS value
TEST_F(LimitRmsTest, LimitRms)
{
    RootComponent root;
    std::string   name             = "limit";
    double const  iteration_period = 1.0;
    LimitRms      limit(name, root, iteration_period);

    double rms_limit         = 5;
    double rms_time_constant = 1;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double first_input = rms_limit - 1;
    ASSERT_TRUE(limit.limit(first_input));

    const double second_input = first_input + pow(rms_limit, 2);
    ASSERT_FALSE(limit.limit(second_input));
}

//! Tests catching value with excessive RMS value coming after a number of entries
TEST_F(LimitRmsTest, LimitRmsLongerRunning)
{
    RootComponent root;
    std::string   name             = "limit";
    double const  iteration_period = 1.0;
    LimitRms      limit(name, root, iteration_period);

    double rms_limit         = 5;
    double rms_time_constant = 1.0;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double first_input = rms_limit - 1;
    ASSERT_TRUE(limit.limit(first_input));
    ASSERT_TRUE(limit.limit(first_input));
    ASSERT_TRUE(limit.limit(first_input));
    ASSERT_TRUE(limit.limit(first_input));
    ASSERT_TRUE(limit.limit(first_input));

    const double second_input = first_input + pow(rms_limit, 2);
    ASSERT_FALSE(limit.limit(second_input));
}

//! Tests catching warning when infinity is provided as input
TEST_F(LimitRmsTest, LimitRmsInfInput)
{
    RootComponent root;
    std::string   name             = "limit";
    const double  iteration_period = 1.0;
    LimitRms      limit(name, root, iteration_period);

    const double rms_limit         = 5;
    const double rms_time_constant = 5e-5;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double input = std::numeric_limits<double>::infinity();
    ASSERT_FALSE(limit.limit(input));
}

//! Tests catching warning when minus infinity is provided as input
TEST_F(LimitRmsTest, LimitRmsMinusInfInput)
{
    RootComponent root;
    std::string   name             = "limit";
    const double  iteration_period = 1.0;
    LimitRms      limit(name, root, iteration_period);

    double rms_limit         = 5;
    double rms_time_constant = 5e-5;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double input = -std::numeric_limits<double>::infinity();
    ASSERT_FALSE(limit.limit(input));
}

//! Tests catching warning when NaN is provided as input
TEST_F(LimitRmsTest, LimitRmsNanInput)
{
    RootComponent root;
    std::string   name = "limit";
    LimitRms      limit(name, root);

    double rms_limit         = 5;
    double rms_time_constant = 1e-4;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double input = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE(limit.limit(input));
}