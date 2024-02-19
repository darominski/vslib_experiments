//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

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
    EXPECT_EQ(serialized["parameters"].size(), 5);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayInt32");
    EXPECT_EQ(serialized["parameters"][3]["name"], "integral_limit");
    EXPECT_EQ(serialized["parameters"][3]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][4]["name"], "rms_threshold");
    EXPECT_EQ(serialized["parameters"][4]["type"], "Float64");
}


// //! Tests default construction of unsigned integral type Limit component
// TEST_F(LimitTest, LimitUnsignedIntegralDefault)
// {
//     std::string name = "uint_limit";
//     Limit<uint32_t> uint_limit(name, nullptr);

// }

// //! Tests default construction of float type Limit component
// TEST_F(LimitTest, LimitFloatDefault)
// {
//     std::string name = "float_limit";
//     Limit<float> float_limit(name, nullptr);

// }

// //! Tests default construction of integral type Limit component
// TEST_F(LimitTest, LimitDoubleDefault)
// {
//     std::string name = "dbl_limit";
//     Limit<double> double_limit(name, nullptr);

// }