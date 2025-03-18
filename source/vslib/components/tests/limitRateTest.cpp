//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "limitRate.hpp"
#include "rootComponent.hpp"
#include "staticJson.hpp"

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

// ************************************************************
// Construction and serialization tests

//! Tests default construction of integral type LimitRate component
TEST_F(LimitRateTest, LimitRateIntegralDefault)
{
    RootComponent      root;
    std::string        name = "limit";
    LimitRate<int32_t> limit(name, root);
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
    RootComponent    root;
    std::string      name = "limit";
    LimitRate<float> limit(name, root);
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
    RootComponent     root;
    std::string       name = "limit";
    LimitRate<double> limit(name, root);
    EXPECT_EQ(limit.getName(), name);

    auto serialized = limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRate");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 1);
    EXPECT_EQ(serialized["parameters"][0]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
}

// ************************************************************
// Tests of limits

//! Tests catching value with excessive rate of change zone with int type
TEST_F(LimitRateTest, LimitRateInt)
{
    RootComponent  root;
    std::string    name = "limit";
    LimitRate<int> limit(name, root);

    const int change_rate = 10;

    set_limit_parameters<int>(limit, change_rate);

    const int  first_input  = 5;
    const auto first_output = limit.limit(first_input, 1.0);
    ASSERT_EQ(first_output, first_input);

    float      second_input  = first_input + change_rate + 1;
    const auto second_output = limit.limit(second_input, 1.0);
    ASSERT_NE(second_output, second_input);
    ASSERT_EQ(second_output, first_input + change_rate * 1.0);
}

//! Tests catching value with excessive negative rate of change zone with int type
TEST_F(LimitRateTest, LimitRateNegativeInt)
{
    RootComponent  root;
    std::string    name = "limit";
    LimitRate<int> limit(name, root);

    const int change_rate = 10;

    set_limit_parameters<int>(limit, change_rate);

    int        first_input  = -5;
    const auto first_output = limit.limit(first_input, 1.0);
    ASSERT_EQ(first_input, first_output);

    float      second_input  = first_input - change_rate - 1;
    const auto second_output = limit.limit(second_input, 1.0);
    ASSERT_NE(second_output, second_input);
    ASSERT_EQ(second_output, first_input + change_rate * 1.0);
}

//! Tests catching value with excessive rate of change zone with double type
TEST_F(LimitRateTest, LimitRateFloat)
{
    RootComponent    root;
    std::string      name = "limit";
    LimitRate<float> limit(name, root);

    const float change_rate = 1.0;

    set_limit_parameters<float>(limit, change_rate);

    float      first_input  = 2.0;
    const auto first_output = limit.limit(first_input, 0.01);
    ASSERT_EQ(first_input, first_output);

    float      second_input  = first_input + change_rate + 1;
    const auto second_output = limit.limit(second_input, 1.0);
    ASSERT_NE(second_input, second_output);
    ASSERT_EQ(second_output, first_input + change_rate * 1.0);
}

//! Tests catching value with excessive negative rate of change zone with double type
TEST_F(LimitRateTest, LimitRateNegativeFloat)
{
    RootComponent    root;
    std::string      name = "limit";
    LimitRate<float> limit(name, root);

    const float change_rate = 1.0;

    set_limit_parameters<float>(limit, change_rate);

    float      first_input  = -2.0;
    const auto first_output = limit.limit(first_input, 0.01);
    ASSERT_EQ(first_input, first_output);

    float      second_input  = first_input - change_rate - 1;
    const auto second_output = limit.limit(second_input, 1.0);
    ASSERT_NE(second_input, second_output);
    ASSERT_EQ(second_output, first_input + change_rate * 1.0);
}

//! Tests catching value with excessive rate of change zone with double type
TEST_F(LimitRateTest, LimitRateDouble)
{
    RootComponent     root;
    std::string       name = "limit";
    LimitRate<double> limit(name, root);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double       first_input  = 2.0;
    double const first_output = limit.limit(first_input, 0.01);
    ASSERT_EQ(first_input, first_output);

    float      second_input  = first_input + change_rate + 1;
    const auto second_output = limit.limit(second_input, 0.01);
    ASSERT_NE(second_input, second_output);
    ASSERT_EQ(second_output, first_input + 0.01 * change_rate);
}

//! Tests catching value with excessive negative rate of change zone with double type
TEST_F(LimitRateTest, LimitRateNegativeDouble)
{
    RootComponent     root;
    std::string       name = "limit";
    LimitRate<double> limit(name, root);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double       first_input  = -2.0;
    double const first_output = limit.limit(first_input, 0.01);
    ASSERT_EQ(first_input, first_output);

    float      second_input  = first_input - change_rate - 1;
    const auto second_output = limit.limit(second_input, 0.01);
    ASSERT_NE(second_input, second_output);
    ASSERT_EQ(second_output, first_input + 0.01 * change_rate);
}

//! Tests catching input with time difference of zero with the last provided value
TEST_F(LimitRateTest, LimitRateZeroTimeDifference)
{
    RootComponent     root;
    std::string       name = "limit";
    LimitRate<double> limit(name, root);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double     input  = 2.0;
    const auto output = limit.limit(input, 0.0);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, std::numeric_limits<double>::max());
}

//! Tests that an expected warning is raised when inf input is provided
TEST_F(LimitRateTest, LimitRateInf)
{
    RootComponent     root;
    std::string       name = "limit";
    LimitRate<double> limit(name, root);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double const first_input = 1.0;
    auto         output      = limit.limit(first_input, 0.1);
    ASSERT_EQ(first_input, output);

    double second_input = std::numeric_limits<double>::infinity();
    output              = limit.limit(second_input, 0.1);
    ASSERT_NE(second_input, output);
    ASSERT_EQ(output, first_input + 0.1 * change_rate);
}

//! Tests that an expected warning is raised when -inf input is provided
TEST_F(LimitRateTest, LimitRateMinusInf)
{
    RootComponent     root;
    std::string       name = "limit";
    LimitRate<double> limit(name, root);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    double const first_input = 1.0;
    auto         output      = limit.limit(first_input, 0.1);
    ASSERT_EQ(first_input, output);

    double second_input = -std::numeric_limits<double>::infinity();
    output              = limit.limit(second_input, 0.1);
    ASSERT_NE(second_input, output);
    ASSERT_EQ(output, first_input + 0.1 * change_rate);
}

//! Tests that an expected warning is raised when NaN input is provided
TEST_F(LimitRateTest, LimitRateNaN)
{
    RootComponent     root;
    std::string       name = "limit";
    LimitRate<double> limit(name, root);

    const double change_rate = 1.0;

    set_limit_parameters<double>(limit, change_rate);

    const double first_input  = 1.0;
    const auto   first_output = limit.limit(first_input, 0.1);
    ASSERT_EQ(first_input, first_output);

    double       second_input  = std::numeric_limits<double>::quiet_NaN();
    const double second_output = limit.limit(second_input, 0.1);
    ASSERT_NE(second_output, second_input);
    ASSERT_EQ(second_output, std::numeric_limits<double>::min());
}
