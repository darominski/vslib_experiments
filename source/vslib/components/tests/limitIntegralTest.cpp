//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "limitIntegral.h"
#include "staticJson.h"

using namespace vslib;

class LimitIntegralTest : public ::testing::Test
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
    std::string            name = "int_limit";
    LimitIntegral<int32_t> integral_limit(name, nullptr);
    EXPECT_EQ(integral_limit.getName(), name);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(integral_limit.getFullName()), registry.getComponents().end());

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
    std::string             name = "uint_limit";
    LimitIntegral<uint32_t> uint_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(uint_limit.getFullName()), registry.getComponents().end());

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
    std::string          name = "float_limit";
    LimitIntegral<float> float_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(float_limit.getFullName()), registry.getComponents().end());

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
    std::string           name = "dbl_limit";
    LimitIntegral<double> double_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(double_limit.getFullName()), registry.getComponents().end());

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
    std::string        name = "limit";
    LimitIntegral<int> limit(name, nullptr);

    int    integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<int>(limit, integral_limit, time_window_length);

    int first_input = integral_limit - 1;
    ASSERT_FALSE(limit.limit(first_input));

    float      second_input = first_input;
    const auto warning      = limit.limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 99 leads to overflow of the integral limit of 100.\n");
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralUInt)
{
    std::string             name = "limit";
    LimitIntegral<uint32_t> limit(name, nullptr);

    uint32_t integral_limit     = 100;
    size_t   time_window_length = 5;

    set_limit_parameters<uint32_t>(limit, integral_limit, time_window_length);

    uint32_t first_input = integral_limit - 1;
    ASSERT_FALSE(limit.limit(first_input));

    uint32_t   second_input = first_input;
    const auto warning      = limit.limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 99 leads to overflow of the integral limit of 100.\n");
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralFloat)
{
    std::string          name = "limit";
    LimitIntegral<float> limit(name, nullptr);

    float  integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<float>(limit, integral_limit, time_window_length);

    float first_input = integral_limit - 1;
    ASSERT_FALSE(limit.limit(first_input));

    float      second_input = first_input;
    const auto warning      = limit.limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 99 leads to overflow of the integral limit of 100.\n");
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralDouble)
{
    std::string           name = "limit";
    LimitIntegral<double> limit(name, nullptr);

    double integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<double>(limit, integral_limit, time_window_length);

    double first_input = integral_limit - 1;
    ASSERT_FALSE(limit.limit(first_input));

    float      second_input = first_input;
    const auto warning      = limit.limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 99 leads to overflow of the integral limit of 100.\n");
}

//! Tests catching value with excessive integrated value
TEST_F(LimitIntegralTest, LimitIntegralIntWrapAround)
{
    std::string        name = "limit";
    LimitIntegral<int> limit(name, nullptr);

    int    integral_limit     = 100;
    size_t time_window_length = 4;

    set_limit_parameters<int>(limit, integral_limit, time_window_length);

    int first_input = 10;
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));

    float      overflow_input = integral_limit - first_input * 3 + 1;
    const auto warning        = limit.limit(overflow_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 71 leads to overflow of the integral limit of 100.\n");
}

//! Tests catching excessive infinite value
TEST_F(LimitIntegralTest, LimitIntegralInfDouble)
{
    std::string           name = "limit";
    LimitIntegral<double> limit(name, nullptr);

    double integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<double>(limit, integral_limit, time_window_length);

    double     input   = std::numeric_limits<double>::infinity();
    const auto warning = limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: inf leads to overflow of the integral limit of 100.\n");
}

//! Tests catching excessive infinite value
TEST_F(LimitIntegralTest, LimitIntegralNaNDouble)
{
    std::string           name = "limit";
    LimitIntegral<double> limit(name, nullptr);

    double integral_limit     = 100;
    size_t time_window_length = 5;

    set_limit_parameters<double>(limit, integral_limit, time_window_length);

    double     input   = std::numeric_limits<double>::quiet_NaN();
    const auto warning = limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value is NaN.\n");
}