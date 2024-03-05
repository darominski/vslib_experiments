//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "componentRegistry.h"
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
        ComponentRegistry& component_registry = ComponentRegistry::instance();
        component_registry.clearRegistry();
        ParameterRegistry& parameter_registry = ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    template<size_t BufferLength = 16>
    void set_limit_parameters(LimitRms<BufferLength>& limit, double rms, size_t rms_time_constant)
    {
        StaticJson rms_val = rms;
        limit.rms_limit.setJsonValue(rms_val);

        StaticJson rms_time_constant_val = rms_time_constant;
        limit.rms_time_constant.setJsonValue(rms_time_constant_val);

        BufferSwitch::flipState();
        limit.verifyParameters();
    }
};

//! Tests default construction of integral type Limit component
TEST_F(LimitRmsTest, LimitRmsDefault)
{
    std::string name = "limit";
    LimitRms    integral_limit(name, nullptr);
    EXPECT_EQ(integral_limit.getName(), name);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(integral_limit.getFullName()), registry.getComponents().end());

    auto serialized = integral_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRms");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "rms_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][1]["name"], "rms_time_constant");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt64");
}

//! Tests catching value with excessive RMS value
TEST_F(LimitRmsTest, LimitRms)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms        = 5;
    size_t rms_window = 2;

    set_limit_parameters(limit, rms, rms_window);

    double first_input = rms;
    ASSERT_FALSE(limit.limit(first_input));

    float      second_input = first_input + rms * rms;
    const auto warning      = limit.limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 30 deviates too far from the RMS limit of 5.\n");
}

//! Tests catching value with excessive RMS value with wrap-around of internal buffer
TEST_F(LimitRmsTest, LimitRmsWrapAround)
{
    std::string name = "limit";
    LimitRms    limit(name, nullptr);

    double rms        = 5;
    size_t rms_window = 4;

    set_limit_parameters(limit, rms, rms_window);

    double first_input = 5;
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));
    ASSERT_FALSE(limit.limit(first_input));

    float      second_input = first_input + rms * rms;
    const auto warning      = limit.limit(second_input);
    ASSERT_TRUE(warning.has_value());
    EXPECT_EQ(warning.value().warning_str, "Value: 30 deviates too far from the RMS limit of 5.\n");
}