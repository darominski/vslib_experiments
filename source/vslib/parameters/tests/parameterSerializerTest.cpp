//! @file
//! @brief File with unit tests of ParameterSerializer class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.h"
#include "json/json.hpp"
#include "parameter.h"
#include "parameterRegistry.h"
#include "parameterSerializer.h"
#include "rootComponent.h"

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
    }

    void TearDown() override
    {
    }
};

namespace
{
    class MockComponent : public Component
    {
      public:
        MockComponent(RootComponent& parent)
            : Component("mockType", "mock_name", parent)
        {
        }
    };
}

// ************************************************************
// Tests of serialization of various Parameter types

// Basic boolean parameter serialization
TEST_F(ParameterTest, BoolParameterSerialization)
{
    RootComponent       root;
    MockComponent       component(root);   // component to attach parameters to
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
    RootComponent       root;
    MockComponent       component(root);   // component to attach parameters to
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
    RootComponent     root;
    MockComponent     component(root);   // component to attach parameters to
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
    RootComponent          root;
    MockComponent          component(root);   // component to attach parameters to
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
    RootComponent       root;
    MockComponent       component(root);   // component to attach parameters to
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
    RootComponent                              root;
    MockComponent                              component(root);   // component to attach parameters to
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
    RootComponent       root;
    MockComponent       component(root);   // component to attach parameters to
    const std::string   parameter_name = "bool";
    Parameter<bool>     parameter(component, parameter_name);
    ParameterSerializer serializer;

    bool           new_value = false;
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), false);
    component.flipBufferState();   // switches between read and background buffer
    parameter.syncWriteBuffer();   // synchronises inactive buffer with active one

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
    RootComponent                    root;
    MockComponent                    component(root);   // component to attach parameters to
    const std::string                parameter_name = "double_array";
    Parameter<std::array<double, 3>> parameter(component, parameter_name, -5, 5);
    ParameterSerializer              serializer;

    std::array<double, 3> new_value = {0.1, 1.2, 2.3};
    nlohmann::json        command   = {{"value", new_value}};
    auto                  output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), false);
    component.flipBufferState();
    parameter.syncWriteBuffer();   // synchronises inactive buffer with active one

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
    RootComponent     root;
    MockComponent     component(root);   // component to attach parameters to
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
    component.flipBufferState();
    parameter.syncWriteBuffer();   // synchronises inactive buffer with active one

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

//! Tests serialization of std::array of enum Parameter when value has been set
TEST_F(ParameterTest, EnumArrayParameterSerializationWithValue)
{
    RootComponent     root;
    MockComponent     component(root);   // component to attach parameters to
    const std::string parameter_name = "enum";
    enum class TestEnum
    {
        field1,
        field2,
        field3
    };
    constexpr size_t                              array_length = 4;
    Parameter<std::array<TestEnum, array_length>> parameter(component, parameter_name);
    ParameterSerializer                           serializer;

    std::array<std::string, array_length> new_value{
        "field2", "field2", "field2", "field2"};   // enums are serialized as strings
    nlohmann::json command = {{"value", new_value}};
    auto           output  = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    component.flipBufferState();
    parameter.syncWriteBuffer();   // synchronises inactive buffer with active one

    EXPECT_EQ(parameter.value()[0], TestEnum::field2);   // tests explicit access
    EXPECT_EQ(parameter.value()[1], TestEnum::field2);   // tests explicit access
    EXPECT_EQ(parameter.value()[2], TestEnum::field2);   // tests explicit access
    EXPECT_EQ(parameter.value()[3], TestEnum::field2);   // tests explicit access

    auto const& serialized_parameter = serializer.serialize(static_cast<std::reference_wrapper<IParameter>>(parameter));
    EXPECT_TRUE(serialized_parameter.is_object());
    EXPECT_TRUE(serialized_parameter.contains("length"));
    EXPECT_TRUE(serialized_parameter.contains("value"));
    EXPECT_TRUE(serialized_parameter.contains("name"));
    EXPECT_TRUE(serialized_parameter.contains("type"));
    EXPECT_TRUE(serialized_parameter.contains("fields"));
    EXPECT_EQ(serialized_parameter["length"], 4);
    EXPECT_EQ(serialized_parameter["value"].dump(), "[\"field2\",\"field2\",\"field2\",\"field2\"]");
    EXPECT_EQ(serialized_parameter["name"], parameter_name);
    EXPECT_EQ(serialized_parameter["type"], "ArrayEnum");
    EXPECT_TRUE(serialized_parameter["fields"].is_array());
    nlohmann::json fields = {"field1", "field2", "field3"};
    EXPECT_EQ(serialized_parameter["fields"], fields);
}
