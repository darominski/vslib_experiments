//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "limitIntegral.hpp"
#include "mockRoot.hpp"
#include "staticJson.hpp"

using namespace vslib;

class LimitIntegralTest : public ::testing::Test
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
    void set_limit_parameters(LimitIntegral<T>& limit, T integral_limit = T{}, size_t integral_limit_window_length = 0)
    {
        StaticJson integral_limit_value = integral_limit;
        limit.integral_limit.setJsonValue(integral_limit_value);

        StaticJson integral_limit_window_length_val = integral_limit_window_length;
        limit.integral_limit_window_length.setJsonValue(integral_limit_window_length_val);

        limit.flipBufferState();
        limit.verifyParameters();
        limit.synchroniseParameterBuffers();
    }
};

//! Tests default construction of integral type LimitIntegral component
TEST_F(LimitIntegralTest, LimitIntegralIntDefault)
{
    MockRoot               root;
    std::string            name = "int_limit";
    LimitIntegral<int32_t> integral_limit(name, root);
    EXPECT_EQ(integral_limit.getName(), name);

    auto serialized = integral_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitIntegral");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt64");
}

//! Tests default construction of unsigned integral type Limit component
TEST_F(LimitIntegralTest, LimitIntegralUintDefault)
{
    MockRoot                root;
    std::string             name = "uint_limit";
    LimitIntegral<uint32_t> uint_limit(name, root);

    auto serialized = uint_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitIntegral");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][0]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt64");
}

//! Tests default construction of float type Limit component
TEST_F(LimitIntegralTest, LimitIntegralFloatDefault)
{
    MockRoot             root;
    std::string          name = "float_limit";
    LimitIntegral<float> float_limit(name, root);

    auto serialized = float_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitIntegral");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt64");
}

//! Tests default construction of integral type Limit component
TEST_F(LimitIntegralTest, LimitIntegralDoubleDefault)
{
    MockRoot              root;
    std::string           name = "dbl_limit";
    LimitIntegral<double> double_limit(name, root);

    auto serialized = double_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitIntegral");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][1]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt64");
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralInt)
{
    MockRoot           root;
    std::string        name = "limit";
    LimitIntegral<int> limit(name, root);

    int    integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<int>(limit, integral_limit, time_window_length);

    const int first_input = integral_limit - 1;
    ASSERT_TRUE(limit.limit(first_input));

    const int second_input = first_input;
    ASSERT_FALSE(limit.limit(second_input));
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralUInt)
{
    MockRoot                root;
    std::string             name = "limit";
    LimitIntegral<uint32_t> limit(name, root);

    uint32_t integral_limit     = 100;
    size_t   time_window_length = 5;

    set_limit_parameters<uint32_t>(limit, integral_limit, time_window_length);

    const uint32_t first_input = integral_limit - 1;
    ASSERT_TRUE(limit.limit(first_input));

    uint32_t second_input = first_input;
    ASSERT_FALSE(limit.limit(second_input));
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralFloat)
{
    MockRoot             root;
    std::string          name = "limit";
    LimitIntegral<float> limit(name, root);

    float  integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<float>(limit, integral_limit, time_window_length);

    const float first_input = integral_limit - 1;
    ASSERT_TRUE(limit.limit(first_input));

    const float second_input = first_input;
    ASSERT_FALSE(limit.limit(second_input));
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralDouble)
{
    MockRoot              root;
    std::string           name = "limit";
    LimitIntegral<double> limit(name, root);

    double integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<double>(limit, integral_limit, time_window_length);

    const double first_input = integral_limit - 1;
    ASSERT_TRUE(limit.limit(first_input));

    const double second_input = first_input;
    ASSERT_FALSE(limit.limit(second_input));
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralIntWrapAround)
{
    MockRoot           root;
    std::string        name = "limit";
    LimitIntegral<int> limit(name, root);

    int    integral_limit     = 100;
    size_t time_window_length = 4;

    set_limit_parameters<int>(limit, integral_limit, time_window_length);

    const int input = 10;
    for (int index = 0; index < 5; index++)
    {
        ASSERT_TRUE(limit.limit(input));
    }

    const int overflow_input = integral_limit - input * 3 + 1;   // 71
    ASSERT_FALSE(limit.limit(overflow_input));
}

//! Tests catching excessive infinite value
TEST_F(LimitIntegralTest, LimitIntegralInfDouble)
{
    MockRoot              root;
    std::string           name = "limit";
    LimitIntegral<double> limit(name, root);

    double integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<double>(limit, integral_limit, time_window_length);

    const double input = std::numeric_limits<double>::infinity();
    ASSERT_FALSE(limit.limit(input));
}

//! Tests catching excessive infinite value
TEST_F(LimitIntegralTest, LimitIntegralNaNFloat)
{
    MockRoot             root;
    std::string          name = "limit";
    LimitIntegral<float> limit(name, root);

    float  integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<float>(limit, integral_limit, time_window_length);

    const float input = std::numeric_limits<float>::quiet_NaN();
    ASSERT_FALSE(limit.limit(input));
}

//! Tests catching excessive infinite value
TEST_F(LimitIntegralTest, LimitIntegralNaNDouble)
{
    MockRoot              root;
    std::string           name = "limit";
    LimitIntegral<double> limit(name, root);

    double integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<double>(limit, integral_limit, time_window_length);

    const double input = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE(limit.limit(input));
}
