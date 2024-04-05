//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "limitRate.h"
#include "staticJson.h"

using namespace vslib;

class LimitRateTest : public ::testing::Test
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

    template<typename T>
    void set_limit_parameters(LimitRate<T>& limit, T change_rate = T{})
    {
        StaticJson change_rate_val = change_rate;
        limit.change_rate.setJsonValue(change_rate_val);

        limit.flipBufferState();
        limit.verifyParameters();
        limit.synchroniseParameterBuffers();
    }
};

//! Tests default construction of integral type LimitRate component
TEST_F(LimitRateTest, LimitRateIntegralDefault)
{
    std::string        name = "limit";
    LimitRate<int32_t> limit(name, nullptr);
    EXPECT_EQ(limit.getName(), name);

    auto serialized = limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRate");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 1);
    EXPECT_EQ(serialized["parameters"][0]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Int32");
}

//! Tests default construction of float type LimitRate component
TEST_F(LimitRateTest, LimitRateFloatDefault)
{
    std::string      name = "limit";
    LimitRate<float> limit(name, nullptr);
    EXPECT_EQ(limit.getName(), name);

    auto serialized = limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRate");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 1);
    EXPECT_EQ(serialized["parameters"][0]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float32");
}

//! Tests default construction of double type LimitRate component
TEST_F(LimitRateTest, LimitRateDoubleDefault)
{
    std::string       name = "limit";
    LimitRate<double> limit(name, nullptr);
    EXPECT_EQ(limit.getName(), name);

    auto serialized = limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRate");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 1);
    EXPECT_EQ(serialized["parameters"][0]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
}

//! Tests catching value with excessive rate of change zone with int type
TEST_F(LimitRateTest, LimitRateInt)
{
    std::string    name = "limit";
    LimitRate<int> limit(name, nullptr);

    const int change_rate = 10;

    set_limit_parameters<int>(limit, change_rate);

    int first_input = 5;
    ASSERT_FALSE(limit.limit(first_input, 1.0));

    float      second_input = first_input + change_rate + 1;
    const auto warning      = limit.limit(second_input, 1.0);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 16 with rate of 11 is above the maximal rate of change of: 10.\n");
}

//! Tests catching value with excessive negative rate of change zone with int type
TEST_F(LimitRateTest, LimitRateNegativeInt)
{
    std::string    name = "limit";
    LimitRate<int> limit(name, nullptr);

    const int change_rate = 10;

    set_limit_parameters<int>(limit, change_rate);

    int first_input = -5;
    ASSERT_FALSE(limit.limit(first_input, 1.0));

    float      second_input = first_input - change_rate - 1;
    const auto warning      = limit.limit(second_input, 1.0);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: -16 with rate of 11 is above the maximal rate of change of: 10.\n");
}

//! Tests catching value with excessive rate of change zone with double type
TEST_F(LimitRateTest, LimitRateFloat)
{
    std::string      name = "limit";
    LimitRate<float> limit(name, nullptr);

    const float change_rate = 1.0;

    set_limit_parameters<float>(limit, change_rate);

    float first_input = 2.0;
    ASSERT_FALSE(limit.limit(first_input, 0.01));

    float      second_input = first_input + change_rate + 1;
    const auto warning      = limit.limit(second_input, 1.0);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 4 with rate of 2 is above the maximal rate of change of: 1.\n");
}

//! Tests catching value with excessive negative rate of change zone with double type
TEST_F(LimitRateTest, LimitRateNegativeFloat)
{
    std::string      name = "limit";
    LimitRate<float> limit(name, nullptr);

    const float change_rate = 1.0;

    set_limit_parameters<float>(limit, change_rate);

    float first_input = -2.0;
    ASSERT_FALSE(limit.limit(first_input, 0.01));

    float      second_input = first_input - change_rate - 1;
    const auto warning      = limit.limit(second_input, 1.0);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: -4 with rate of 2 is above the maximal rate of change of: 1.\n");
}

//! Tests catching value with excessive rate of change zone with double type
TEST_F(LimitRateTest, LimitRateDouble)
{
    std::string       name = "limit";
    LimitRate<double> limit(name, nullptr);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double first_input = 2.0;
    ASSERT_FALSE(limit.limit(first_input, 0.01));

    float      second_input = first_input + change_rate + 1;
    const auto warning      = limit.limit(second_input, 0.01);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 4 with rate of 200 is above the maximal rate of change of: 1.\n");
}

//! Tests catching value with excessive negative rate of change zone with double type
TEST_F(LimitRateTest, LimitRateNegativeDouble)
{
    std::string       name = "limit";
    LimitRate<double> limit(name, nullptr);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double first_input = -2.0;
    ASSERT_FALSE(limit.limit(first_input, 0.01));

    float      second_input = first_input - change_rate - 1;
    const auto warning      = limit.limit(second_input, 0.01);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: -4 with rate of 200 is above the maximal rate of change of: 1.\n");
}

//! Tests catching input with time difference of zero with the last provided value
TEST_F(LimitRateTest, LimitRateZeroTimeDifference)
{
    std::string       name = "limit";
    LimitRate<double> limit(name, nullptr);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double first_input = 2.0;
    auto   warning     = limit.limit(first_input, 0.0);

    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Time difference is equal to zero in rate limit calculation.\n");
}

//! Tests that an expected warning is raised when inf input is provided
TEST_F(LimitRateTest, LimitRateInf)
{
    std::string       name = "limit";
    LimitRate<double> limit(name, nullptr);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double const first_input = 1.0;
    auto         warning     = limit.limit(first_input, 0.1);
    ASSERT_FALSE(warning.has_value());

    double second_input = std::numeric_limits<double>::infinity();
    warning             = limit.limit(second_input, 0.1);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: inf with rate of inf is above the maximal rate of change of: 1.\n");
}

//! Tests that an expected warning is raised when -inf input is provided
TEST_F(LimitRateTest, LimitRateMinusInf)
{
    std::string       name = "limit";
    LimitRate<double> limit(name, nullptr);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double const first_input = 1.0;
    auto         warning     = limit.limit(first_input, 0.1);
    ASSERT_FALSE(warning.has_value());

    double second_input = -std::numeric_limits<double>::infinity();
    warning             = limit.limit(second_input, 0.1);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: -inf with rate of inf is above the maximal rate of change of: 1.\n");
}

//! Tests that an expected warning is raised when NaN input is provided
TEST_F(LimitRateTest, LimitRateNaN)
{
    std::string       name = "limit";
    LimitRate<double> limit(name, nullptr);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double const first_input = 1.0;
    auto         warning     = limit.limit(first_input, 0.1);
    ASSERT_FALSE(warning.has_value());

    double second_input = std::numeric_limits<double>::quiet_NaN();
    warning             = limit.limit(second_input, 0.1);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value is NaN.\n");
}