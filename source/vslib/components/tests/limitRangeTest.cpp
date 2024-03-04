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

//! Tests catching lower limit violation
TEST_F(LimitRangeTest, LimitRangeMin)
{
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, nullptr);

    const double min = 4;
    const double max = 1e3;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = min - 2;

    const auto warning = double_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 2 is below or equal to the minimal value of 4.\n");
}

//! Tests catching upper limit violation
TEST_F(LimitRangeTest, LimitRangeMax)
{
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, nullptr);

    const double min = 4;
    const double max = 1e2;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = max + 1;

    const auto warning = double_limit.limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 101 is above of equal to the maximal value of 100.\n");
}

//! Tests catching value in the dead zone
TEST_F(LimitRangeTest, LimitRangeDeadZone)
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