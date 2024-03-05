//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "limitRange.h"
#include "staticJson.h"

using namespace vslib;

class LimitRangeTest : public ::testing::Test
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
    void set_limit_parameters(LimitRange<T>& limit, T min = T{}, T max = T{}, std::array<T, 2> dead_zone = {T{}, T{}})
    {
        StaticJson min_val = min;
        limit.min.setJsonValue(min_val);

        StaticJson max_val = max;
        limit.max.setJsonValue(max_val);

        StaticJson dead_zone_val = dead_zone;
        limit.dead_zone.setJsonValue(dead_zone_val);

        BufferSwitch::flipState();
        limit.verifyParameters();
    }
};

// ************************************************************
// Basic construction tests for supported types

//! Tests default construction of integral type LimitRange component
TEST_F(LimitRangeTest, LimitIntegralDefault)
{
    std::string         name = "int_limit";
    LimitRange<int32_t> integral_limit(name, nullptr);
    EXPECT_EQ(integral_limit.getName(), name);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(integral_limit.getFullName()), registry.getComponents().end());

    auto serialized = integral_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayInt32");
}

//! Tests default construction of unsigned integral type LimitRange component
TEST_F(LimitRangeTest, LimitUnsignedIntegralDefault)
{
    std::string          name = "uint_limit";
    LimitRange<uint32_t> uint_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(uint_limit.getFullName()), registry.getComponents().end());

    auto serialized = uint_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayUInt32");
}

//! Tests default construction of float type LimitRange component
TEST_F(LimitRangeTest, LimitRangeFloatDefault)
{
    std::string       name = "float_limit";
    LimitRange<float> float_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(float_limit.getFullName()), registry.getComponents().end());

    auto serialized = float_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayFloat32");
}

//! Tests default construction of integral type LimitRange component
TEST_F(LimitRangeTest, LimitRangeDoubleDefault)
{
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(double_limit.getFullName()), registry.getComponents().end());

    auto serialized = double_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayFloat64");
}

// ************************************************************
// Basic min/max tests for supported types

//! Tests catching lower limit violation with int type
TEST_F(LimitRangeTest, LimitRangeIntMin)
{
    std::string     name = "int_limit";
    LimitRange<int> int_limit(name, nullptr);

    const int min = -10;
    const int max = 10;
    set_limit_parameters<int>(int_limit, min, max);

    const double input = min - 2;

    const auto warning = int_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: -12 is below the minimal value of -10.\n");
}

//! Tests that min limit is exclusive, with int type
TEST_F(LimitRangeTest, LimitRangeIntMinExclusive)
{
    std::string     name = "int_limit";
    LimitRange<int> int_limit(name, nullptr);

    const int min = -10;
    const int max = 10;
    set_limit_parameters<int>(int_limit, min, max);

    const double input = min;

    const auto warning = int_limit.limit(input);
    ASSERT_FALSE(warning.has_value());
}

//! Tests that min limit is exclusive, with float type
TEST_F(LimitRangeTest, LimitRangeFloatMinExclusive)
{
    std::string       name = "float_limit";
    LimitRange<float> float_limit(name, nullptr);

    const float min = -3.14159;
    const float max = 3.14159;
    set_limit_parameters<float>(float_limit, min, max);

    const float input = min;

    const auto warning = float_limit.limit(input);
    ASSERT_FALSE(warning.has_value());
}

//! Tests catching lower limit violation with unsigned int type
TEST_F(LimitRangeTest, LimitRangeUIntMin)
{
    std::string          name = "uint_limit";
    LimitRange<uint64_t> uint_limit(name, nullptr);

    const uint64_t min = 1;
    const uint64_t max = 1e3;
    set_limit_parameters<uint64_t>(uint_limit, min, max);

    const uint64_t input = 0;

    const auto warning = uint_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 0 is below the minimal value of 1.\n");
}

//! Tests catching lower limit violation with double type
TEST_F(LimitRangeTest, LimitRangeDoubleMin)
{
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, nullptr);

    const double min = 4;
    const double max = 1e3;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = min - 2;

    const auto warning = double_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 2 is below the minimal value of 4.\n");
}

//! Tests catching upper limit violation with int type
TEST_F(LimitRangeTest, LimitRangeIntMax)
{
    std::string     name = "int_limit";
    LimitRange<int> int_limit(name, nullptr);

    const int min = 4;
    const int max = 1e2;
    set_limit_parameters<int>(int_limit, min, max);

    const int input = max + 1;

    const auto warning = int_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 101 is above the maximal value of 100.\n");
}

//! Tests catching upper limit violation with uint type
TEST_F(LimitRangeTest, LimitRangeUIntMax)
{
    std::string          name = "uint_limit";
    LimitRange<uint32_t> uint_limit(name, nullptr);

    const uint32_t min = 4;
    const uint32_t max = 1e2;
    set_limit_parameters<uint32_t>(uint_limit, min, max);

    const uint32_t input = max + 1;

    const auto warning = uint_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 101 is above the maximal value of 100.\n");
}

//! Tests catching upper limit violation with double type
TEST_F(LimitRangeTest, LimitRangeDoubleMax)
{
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, nullptr);

    const double min = 4;
    const double max = 1e2;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = max + 1;

    const auto warning = double_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 101 is above the maximal value of 100.\n");
}

// ************************************************************
// Basic dead-zone tests for supported types

//! Tests catching value in the dead zone
TEST_F(LimitRangeTest, LimitRangeIntDeadZone)
{
    std::string     name = "int_limit";
    LimitRange<int> limit(name, nullptr);

    const int          min = -10;
    const int          max = 100;
    std::array<int, 2> dead_zone{0, 3};

    set_limit_parameters<int>(limit, min, max, dead_zone);

    const int input = dead_zone[0] + 1;

    const auto warning = limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 1 is inside the defined dead zone of [0, 3].\n");
}


//! Tests catching value in the dead zone
TEST_F(LimitRangeTest, LimitRangeUIntDeadZone)
{
    std::string          name = "int_limit";
    LimitRange<uint16_t> limit(name, nullptr);

    const uint16_t          min = -10;
    const uint16_t          max = 100;
    std::array<uint16_t, 2> dead_zone{0, 4};

    set_limit_parameters<uint16_t>(limit, min, max, dead_zone);

    const uint16_t input = dead_zone[0] + 1;

    const auto warning = limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 1 is inside the defined dead zone of [0, 4].\n");
}


//! Tests catching value in the dead zone
TEST_F(LimitRangeTest, LimitRangeDoubleDeadZone)
{
    std::string        name = "double_limit";
    LimitRange<double> limit(name, nullptr);

    const double          min = -10;
    const double          max = 100;
    std::array<double, 2> dead_zone{0.0, 3.0};

    set_limit_parameters<double>(limit, min, max, dead_zone);

    const double input = dead_zone[0] + 0.1;

    const auto warning = limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 0.1 is inside the defined dead zone of [0, 3].\n");
}

// ************************************************************
// Tests around numerical limits