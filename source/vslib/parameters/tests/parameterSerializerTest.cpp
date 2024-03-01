//! @file
//! @brief File with unit tests of ParameterSerializer class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "bufferSwitch.h"
#include "component.h"
#include "json/json.hpp"
#include "parameter.h"
#include "parameterRegistry.h"
#include "parameterSerializer.h"

using namespace vslib;

class ParameterTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // cleans up the registry so every test starts anew, otherwise
        // the registry would persist between tests
        ParameterRegistry& registry = ParameterRegistry::instance();
        registry.clearRegistry();
        if (BufferSwitch::getState() != 0)
        {
            BufferSwitch::flipState();   // resets the global buffer switch to known state
        }
    }

    void TearDown() override
    {
    }
};

class MockComponent : public Component
{
  public:
    MockComponent()
        : Component("mockType", "mockName", nullptr)
    {
    }
};

// ************************************************************
// Tests of serialization of various Parameter types

// Basic boolean parameter serialization
TEST_F(ParameterTest, BoolParameterSerialization)
{
    MockComponent       component;   // component to attach parameters to
    const std::string   parameter_name = "bool";
    Parameter<bool>     parameter(component, parameter_name);
    ParameterSerializer serializer;

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_EQ(serialized_parameter["length"], 1);
    EXPECT_EQ(serialized_parameter["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "Bool");
}

// Basic integer parameter serialization
TEST_F(ParameterTest, IntParameterSerialization)
{
    MockComponent       component;   // component to attach parameters to
    const std::string   parameter_name = "int";
    Parameter<uint32_t> parameter(component, parameter_name);
    ParameterSerializer serializer;

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_EQ(serialized_parameter["length"], 1);
    EXPECT_EQ(serialized_parameter["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "UInt32");
}

// Basic enum parameter serialization
TEST_F(ParameterTest, EnumParameterSerialization)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "enum";
    enum class TestEnum
    {
        field1,
        field2,
    };
    Parameter<TestEnum> parameter(component, parameter_name);
    ParameterSerializer serializer;

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_TRUE(serialized_parameter.contains("fields"));
    EXPECT_EQ(serialized_parameter["length"], 2);
    EXPECT_EQ(serialized_parameter["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "Enum");
    EXPECT_TRUE(serialized_parameter["fields"].is_array());
    nlohmann::json fields = {"field1", "field2"};
    EXPECT_EQ(serialized_parameter["fields"], fields);
}

// Basic std::string parameter serialization
TEST_F(ParameterTest, StringParameterSerialization)
{
    MockComponent          component;   // component to attach parameters to
    const std::string      parameter_name = "string";
    Parameter<std::string> parameter(component, parameter_name);
    ParameterSerializer    serializer;

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_EQ(serialized_parameter["length"], 0);
    EXPECT_EQ(serialized_parameter["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "String");
}

// Basic float parameter serialization, with limits
TEST_F(ParameterTest, FloatWithLimitsParameterSerialization)
{
    MockComponent       component;   // component to attach parameters to
    const std::string   parameter_name = "float";
    Parameter<float>    parameter(component, parameter_name, -5, 10);
    ParameterSerializer serializer;

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_TRUE(serialized_parameter.contains("limit_min"));
    EXPECT_TRUE(serialized_parameter.contains("limit_max"));
    EXPECT_EQ(serialized_parameter["length"], 1);
    EXPECT_EQ(serialized_parameter["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "Float32");
    EXPECT_EQ(serialized_parameter["limit_min"], -5);
    EXPECT_EQ(serialized_parameter["limit_max"], 10);
}

// Basic int16 array parameter serialization, with limits
TEST_F(ParameterTest, IntArrayWithLimitsParameterSerialization)
{
    MockComponent                              component;   // component to attach parameters to
    const std::string                          parameter_name = "int16";
    constexpr size_t                           array_size     = 5;
    Parameter<std::array<int16_t, array_size>> parameter(component, parameter_name, -5, 10);
    ParameterSerializer                        serializer;

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_TRUE(serialized_parameter.contains("limit_min"));
    EXPECT_TRUE(serialized_parameter.contains("limit_max"));
    EXPECT_EQ(serialized_parameter["length"], array_size);
    EXPECT_EQ(serialized_parameter["value"], nlohmann::json::array());
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "ArrayInt16");
    EXPECT_EQ(serialized_parameter["limit_min"], -5);
    EXPECT_EQ(serialized_parameter["limit_max"], 10);
}
// ************************************************************
// Test of serialization of Parameter when value was set via JSON

//! Tests serialization of array of double Parameter when value has been set
TEST_F(ParameterTest, BoolParameterSerializationWithValue)
{
    MockComponent       component;   // component to attach parameters to
    const std::string   parameter_name = "bool";
    Parameter<bool>     parameter(component, parameter_name);
    ParameterSerializer serializer;

    bool           new_value = false;
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), false);
    BufferSwitch::flipState();        // switches between read and background buffer
    parameter.syncInactiveBuffer();   // synchronises inactive buffer with active one

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_EQ(serialized_parameter["length"], 1);
    EXPECT_EQ(serialized_parameter["value"], new_value);
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "Bool");
}

//! Tests serialization of array of double Parameter when value has been set
TEST_F(ParameterTest, DoubleArrayParameterSerializationWithValue)
{
    MockComponent                    component;   // component to attach parameters to
    const std::string                parameter_name = "double_array";
    Parameter<std::array<double, 3>> parameter(component, parameter_name, -5, 5);
    ParameterSerializer              serializer;

    std::array<double, 3> new_value = {0.1, 1.2, 2.3};
    nlohmann::json        command   = {{"value", new_value}};
    auto                  output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), false);
    BufferSwitch::flipState();        // switches between read and background buffer
    parameter.syncInactiveBuffer();   // synchronises inactive buffer with active one

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_TRUE(serialized_parameter.contains("limit_min"));
    EXPECT_TRUE(serialized_parameter.contains("limit_max"));
    EXPECT_EQ(serialized_parameter["length"], 3);
    EXPECT_EQ(serialized_parameter["value"], new_value);
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "ArrayFloat64");
    EXPECT_EQ(serialized_parameter["limit_min"], -5);
    EXPECT_EQ(serialized_parameter["limit_max"], 5);
}

//! Tests serialization of enum Parameter when value has been set
TEST_F(ParameterTest, EnumParameterSerializationWithValue)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "enum";
    enum class TestEnum
    {
        field1,
        field2,
        field3
    };
    Parameter<TestEnum> parameter(component, parameter_name);
    ParameterSerializer serializer;

    std::string    new_value = "field2";   // enums are serialized as strings
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    BufferSwitch::flipState();        // switches between read and background buffer
    parameter.syncInactiveBuffer();   // synchronises inactive buffer with active one

    EXPECT_EQ(parameter.value(), TestEnum::field2);   // tests explicit access

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_TRUE(serialized_parameter.contains("fields"));
    EXPECT_EQ(serialized_parameter["length"], 3);
    EXPECT_EQ(serialized_parameter["value"], "field2");
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "Enum");
    EXPECT_TRUE(serialized_parameter["fields"].is_array());
    nlohmann::json fields = {"field1", "field2", "field3"};
    EXPECT_EQ(serialized_parameter["fields"], fields);
}
