//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "limit.h"
#include "staticJson.h"

using namespace vslib;

class LimitTest : public ::testing::Test
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
    void set_limit_parameters(
        Limit<T>& limit, T min = T{}, T max = T{}, std::array<T, 2> dead_zone = {T{}, T{}}, T change_rate = T{},
        T integral_limit = T{}, size_t integral_limit_window_length = 0, T rms = T{}, size_t rms_time_constant = 0
    )
    {
        StaticJson min_val = min;
        limit.min.setJsonValue(min_val);

        StaticJson max_val = max;
        limit.max.setJsonValue(max_val);

        StaticJson dead_zone_val = dead_zone;
        limit.dead_zone.setJsonValue(dead_zone_val);

        StaticJson change_rate_val = change_rate;
        limit.change_rate.setJsonValue(change_rate_val);

        StaticJson integral_limit_value = integral_limit;
        limit.integral_limit.setJsonValue(integral_limit_value);

        StaticJson integral_limit_window_length_val = integral_limit_window_length;
        limit.integral_limit_window_length.setJsonValue(integral_limit_window_length_val);

        StaticJson rms_val = rms;
        limit.rms.setJsonValue(rms_val);

        StaticJson rms_time_constant_val = rms_time_constant;
        limit.rms_time_constant.setJsonValue(rms_time_constant_val);

        BufferSwitch::flipState();
        limit.verifyParameters();
        limit.rms_time_constant.syncInactiveBuffer();
    }
};

//! Tests default construction of integral type Limit component
TEST_F(LimitTest, LimitIntegralDefault)
{
    std::string    name = "int_limit";
    Limit<int32_t> integral_limit(name, nullptr);
    EXPECT_EQ(integral_limit.getName(), name);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(integral_limit.getFullName()), registry.getComponents().end());

    auto serialized = integral_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "Limit");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 8);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayInt32");
    EXPECT_EQ(serialized["parameters"][3]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][3]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][4]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][4]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][5]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][5]["type"], "UInt64");
    EXPECT_EQ(serialized["parameters"][6]["name"], "rms_threshold");
    EXPECT_EQ(serialized["parameters"][6]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][7]["name"], "rms_time_constant");
    EXPECT_EQ(serialized["parameters"][7]["type"], "UInt64");
}

//! Tests default construction of unsigned integral type Limit component
TEST_F(LimitTest, LimitUnsignedIntegralDefault)
{
    std::string     name = "uint_limit";
    Limit<uint32_t> uint_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(uint_limit.getFullName()), registry.getComponents().end());

    auto serialized = uint_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "Limit");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 8);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayUInt32");
    EXPECT_EQ(serialized["parameters"][3]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][3]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][4]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][4]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][5]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][5]["type"], "UInt64");
    EXPECT_EQ(serialized["parameters"][6]["name"], "rms_threshold");
    EXPECT_EQ(serialized["parameters"][6]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][7]["name"], "rms_time_constant");
    EXPECT_EQ(serialized["parameters"][7]["type"], "UInt64");
}

//! Tests default construction of float type Limit component
TEST_F(LimitTest, LimitFloatDefault)
{
    std::string  name = "float_limit";
    Limit<float> float_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(float_limit.getFullName()), registry.getComponents().end());

    auto serialized = float_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "Limit");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 8);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayFloat32");
    EXPECT_EQ(serialized["parameters"][3]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][3]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][4]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][4]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][5]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][5]["type"], "UInt64");
    EXPECT_EQ(serialized["parameters"][6]["name"], "rms_threshold");
    EXPECT_EQ(serialized["parameters"][6]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][7]["name"], "rms_time_constant");
    EXPECT_EQ(serialized["parameters"][7]["type"], "UInt64");
}

//! Tests default construction of integral type Limit component
TEST_F(LimitTest, LimitDoubleDefault)
{
    std::string   name = "dbl_limit";
    Limit<double> double_limit(name, nullptr);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(double_limit.getFullName()), registry.getComponents().end());

    auto serialized = double_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "Limit");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 8);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayFloat64");
    EXPECT_EQ(serialized["parameters"][3]["name"], "change_rate");
    EXPECT_EQ(serialized["parameters"][3]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][4]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][4]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][5]["name"], "integral_limit_time_window");
    EXPECT_EQ(serialized["parameters"][5]["type"], "UInt64");
    EXPECT_EQ(serialized["parameters"][6]["name"], "rms_threshold");
    EXPECT_EQ(serialized["parameters"][6]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][7]["name"], "rms_time_constant");
    EXPECT_EQ(serialized["parameters"][7]["type"], "UInt64");
}

//! Tests catching lower limit violation
TEST_F(LimitTest, LimitMin)
{
    std::string   name = "dbl_limit";
    Limit<double> double_limit(name, nullptr);

    const double min = 4;
    const double max = 1e3;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = min - 2;

    const auto warning = double_limit.check_min_max_limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 2 is below or equal to the minimal value of 4.\n");
}

//! Tests catching upper limit violation
TEST_F(LimitTest, LimitMax)
{
    std::string   name = "dbl_limit";
    Limit<double> double_limit(name, nullptr);

    const double min = 4;
    const double max = 1e2;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = max + 1;

    const auto warning = double_limit.check_min_max_limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 101 is above of equal to the maximal value of 100.\n");
}

//! Tests catching value in the dead zone
TEST_F(LimitTest, LimitDeadZone)
{
    std::string name = "int_limit";
    Limit<int>  limit(name, nullptr);

    const int          min = -10;
    const int          max = 100;
    std::array<int, 2> dead_zone{0, 3};

    set_limit_parameters<int>(limit, min, max, dead_zone);

    const int input = dead_zone[0] + 1;

    const auto warning = limit.check_min_max_limit(input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 1 is inside the defined dead zone of [0, 3].\n");
}

//! Tests catching value with excessive rate of change zone
TEST_F(LimitTest, LimitRateOfChange)
{
    std::string  name = "int_limit";
    Limit<float> limit(name, nullptr);

    const float          min = 1;
    const float          max = 100;
    std::array<float, 2> dead_zone{0, 0};
    const float          change_rate = 10;

    set_limit_parameters<float>(limit, min, max, dead_zone, change_rate);

    float first_input = min + 1;
    ASSERT_FALSE(limit.check_change_rate_limit(first_input));

    float      second_input = first_input + change_rate + 1;
    const auto warning      = limit.check_change_rate_limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(
        warning.value().warning_str, "Value: 13 with difference of 11 is above the maximal rate of change of: 10.\n"
    );
}

//! Tests catching value with excessive integrated value
TEST_F(LimitTest, LimitIntegral)
{
    std::string name = "int_limit";
    Limit<int>  limit(name, nullptr);

    const int          min = 1;
    const int          max = 100;
    std::array<int, 2> dead_zone{0, 0};
    const int          change_rate        = 10;
    int                integral_limit     = 100;
    size_t             time_window_length = 5;

    set_limit_parameters<int>(limit, min, max, dead_zone, change_rate, integral_limit, time_window_length);

    int first_input = max - 1;
    ASSERT_FALSE(limit.check_integral_limit(first_input));

    float      second_input = first_input;
    const auto warning      = limit.check_integral_limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 99 leads to overflow of the integral limit of 100.\n");
}

//! Tests catching value with excessive integrated value
TEST_F(LimitTest, LimitRMS)
{
    std::string   name = "int_limit";
    Limit<double> limit(name, nullptr);

    const double          min = 1;
    const double          max = 100;
    std::array<double, 2> dead_zone{0, 0};
    const double          change_rate        = 10;
    double                integral_limit     = 100;
    size_t                time_window_length = 0;
    double                rms                = 2;
    size_t                rms_window         = 2;

    set_limit_parameters<double>(
        limit, min, max, dead_zone, change_rate, integral_limit, time_window_length, rms, rms_window
    );

    double first_input = 5;
    ASSERT_FALSE(limit.check_rms_limit(first_input));

    float      second_input = first_input + rms * rms;
    const auto warning      = limit.check_rms_limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 9 deviates too far from the RMS limit of 2.\n");
}