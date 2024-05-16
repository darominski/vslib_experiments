//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "limitRms.h"
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
    std::string name = "limit";
    LimitRms    limit(name, nullptr);
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
TEST_F(LimitRmsTest, LimitRmsNonRT)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms_limit         = 5;
    double rms_time_constant = 5e-5;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    double first_input = rms_limit - 1;
    ASSERT_FALSE(limit.limitNonRT(first_input));

    float      second_input = first_input + rms_limit * rms_limit;
    const auto warning      = limit.limitNonRT(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 29 deviates too far from the RMS limit of 5.\n");
}

//! Tests catching value with excessive RMS value
TEST_F(LimitRmsTest, LimitRms)
{
    std::string  name             = "limit";
    double const iteration_period = 1.0;
    LimitRms     limit(name, nullptr, iteration_period);

    double rms_limit         = 5;
    double rms_time_constant = 1;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double first_input = rms_limit - 1;
    ASSERT_TRUE(limit.limit(first_input));

    const double second_input = first_input + pow(rms_limit, 2);
    ASSERT_FALSE(limit.limit(second_input));
}

//! Tests catching value with excessive RMS value coming after a number of entries
TEST_F(LimitRmsTest, LimitRmsLongerRunningNonRT)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms               = 5;
    double rms_time_constant = 5e-5;

    set_limit_parameters(limit, rms, rms_time_constant);

    double first_input = rms - 1;
    ASSERT_FALSE(limit.limitNonRT(first_input).has_value());
    ASSERT_FALSE(limit.limitNonRT(first_input).has_value());
    ASSERT_FALSE(limit.limitNonRT(first_input).has_value());
    ASSERT_FALSE(limit.limitNonRT(first_input).has_value());
    ASSERT_FALSE(limit.limitNonRT(first_input).has_value());

    float      second_input = first_input + rms * rms;
    const auto warning      = limit.limitNonRT(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 29 deviates too far from the RMS limit of 5.\n");
}

//! Tests catching value with excessive RMS value coming after a number of entries
TEST_F(LimitRmsTest, LimitRmsLongerRunning)
{
    std::string  name             = "limit";
    double const iteration_period = 1.0;
    LimitRms     limit(name, nullptr, iteration_period);

    double rms_limit         = 5;
    double rms_time_constant = 1.0;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double filter_factor = iteration_period / (rms_time_constant + 0.5 * iteration_period);

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
TEST_F(LimitRmsTest, LimitRmsInfInputNonRT)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms               = 5;
    double rms_time_constant = 5e-5;

    set_limit_parameters(limit, rms, rms_time_constant);

    double     input   = std::numeric_limits<double>::infinity();
    const auto warning = limit.limitNonRT(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: inf deviates too far from the RMS limit of 5.\n");
}

//! Tests catching warning when infinity is provided as input
TEST_F(LimitRmsTest, LimitRmsInfInput)
{
    std::string  name             = "limit";
    const double iteration_period = 1.0;
    LimitRms     limit(name, nullptr, iteration_period);

    const double rms_limit         = 5;
    const double rms_time_constant = 5e-5;
    const double filter_factor     = iteration_period / (rms_time_constant + 0.5 * iteration_period);

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double input = std::numeric_limits<double>::infinity();
    ASSERT_FALSE(limit.limit(input));
}

//! Tests catching warning when minus infinity is provided as input
TEST_F(LimitRmsTest, LimitRmsMinusInfInputNonRT)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms               = 5;
    double rms_time_constant = 5e-5;

    set_limit_parameters(limit, rms, rms_time_constant);

    double     input   = -std::numeric_limits<double>::infinity();
    const auto warning = limit.limitNonRT(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: -inf deviates too far from the RMS limit of 5.\n");
}

//! Tests catching warning when minus infinity is provided as input
TEST_F(LimitRmsTest, LimitRmsMinusInfInput)
{
    std::string  name             = "limit";
    const double iteration_period = 1.0;
    LimitRms     limit(name, nullptr, iteration_period);

    double       rms_limit         = 5;
    double       rms_time_constant = 5e-5;
    const double filter_factor     = iteration_period / (rms_time_constant + 0.5 * iteration_period);

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double input = -std::numeric_limits<double>::infinity();
    ASSERT_FALSE(limit.limit(input));
}

//! Tests catching warning when NaN is provided as input
TEST_F(LimitRmsTest, LimitRmsNanInputNonRT)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms               = 5;
    double rms_time_constant = 1e-4;

    set_limit_parameters(limit, rms, rms_time_constant);

    double     input   = std::numeric_limits<double>::quiet_NaN();
    const auto warning = limit.limitNonRT(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value is a NaN.\n");
}

//! Tests catching warning when NaN is provided as input
TEST_F(LimitRmsTest, LimitRmsNanInput)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms_limit         = 5;
    double rms_time_constant = 1e-4;

    set_limit_parameters(limit, rms_limit, rms_time_constant);

    const double input = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE(limit.limit(input));
}