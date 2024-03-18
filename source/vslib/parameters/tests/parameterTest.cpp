//! @file
//! @brief File with unit tests of Parameter class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.h"
#include "json/json.hpp"
#include "parameter.h"
#include "parameterRegistry.h"

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

class MockComponent : public Component
{
  public:
    MockComponent()
        : Component("mockType", "mockName", nullptr)
    {
    }
};

// ************************************************************
// Tests of basic Parameter creation of all supported types

//! Checks that boolean Parameter can be registered to a component
TEST_F(ParameterTest, BoolParameterDefinition)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "bool";
    Parameter<bool>   parameter(component, parameter_name);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
}

//! Checks that integer Parameter can be registered to a component
TEST_F(ParameterTest, IntParameterDefinition)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "int";
    Parameter<int>    parameter(component, parameter_name);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
}

//! Checks that float Parameter can be registered to a component
TEST_F(ParameterTest, FloatParameterDefinition)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "float";
    Parameter<float>  parameter(component, parameter_name);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
}

//! Checks that std::string Parameter can be registered to a component
TEST_F(ParameterTest, StringParameterDefinition)
{
    MockComponent          component;   // component to attach parameters to
    const std::string      parameter_name = "string";
    Parameter<std::string> parameter(component, parameter_name);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
}
//! Checks that enum Parameter can be registered to a component
TEST_F(ParameterTest, EnumParameterDefinition)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "enum";
    enum class TestEnum
    {
        field1,
        field2
    };
    Parameter<TestEnum> parameter(component, parameter_name);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
}

//! Checks that double Parameter with limits can be attached to a component
TEST_F(ParameterTest, DoubleParameterWithLimits)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "double";
    Parameter<double> parameter(component, parameter_name, -10, 10);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
    EXPECT_TRUE((std::is_same_v<decltype(parameter.getLimitMin()), const double&>));
    EXPECT_TRUE((std::is_same_v<decltype(parameter.getLimitMax()), const double&>));
    EXPECT_EQ(parameter.getLimitMin(), -10);
    EXPECT_EQ(parameter.getLimitMax(), 10);
}

//! Checks that array of int Parameter with limits can be attached to a component
TEST_F(ParameterTest, IntArrayParameterWithLimits)
{
    MockComponent                      component;   // component to attach parameters to
    const std::string                  parameter_name = "array_int";
    Parameter<std::array<uint64_t, 5>> parameter(component, parameter_name, -1, 1);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
    EXPECT_TRUE((std::is_same_v<decltype(parameter.getLimitMin()), const uint64_t&>));
    EXPECT_TRUE((std::is_same_v<decltype(parameter.getLimitMax()), const uint64_t&>));
    EXPECT_EQ(parameter.getLimitMin(), -1);
    EXPECT_EQ(parameter.getLimitMax(), 1);
}

//! Checks that array of std::string Parameter can be attached to a component
TEST_F(ParameterTest, StringArrayParameter)
{
    MockComponent                         component;   // component to attach parameters to
    const std::string                     parameter_name = "array_string";
    Parameter<std::array<std::string, 5>> parameter(component, parameter_name);
    EXPECT_EQ(parameter.getName(), parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());
}

// ************************************************************
// Tests of setting the Parameter's value via JSON

// Tests setting value to int Parameter from a JSON command
TEST_F(ParameterTest, BoolParameterSetValue)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "bool";
    Parameter<bool>   parameter(component, parameter_name, -5, 10);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    bool           new_value = true;
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    component.flipBufferState();

    EXPECT_EQ(parameter.value(), new_value);
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

// Tests setting value to int Parameter from a JSON command
TEST_F(ParameterTest, IntParameterSetValue)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "int";
    Parameter<int>    parameter(component, parameter_name, -5, 10);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    int            new_value = 2;
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    component.flipBufferState();

    EXPECT_EQ(parameter.value(), new_value);
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

//! Tests setting value to double Parameter from a JSON command
TEST_F(ParameterTest, DoubleParameterSetValue)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "double";
    Parameter<double> parameter(component, parameter_name, -1, 5);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    double         new_value = 3.14159;
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    component.flipBufferState();

    EXPECT_EQ(parameter.value(), new_value);
    EXPECT_EQ(parameter, new_value);               // tests implicit conversion operator
    EXPECT_EQ(parameter + 1.1, new_value + 1.1);   // tests access to parameter as if it was double
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

//! Tests setting value to std::string Parameter from a JSON command
TEST_F(ParameterTest, StringParameterSetValue)
{
    MockComponent          component;   // component to attach parameters to
    const std::string      parameter_name = "string";
    Parameter<std::string> parameter(component, parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    std::string    new_value = "text";
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    component.flipBufferState();

    EXPECT_EQ(parameter.value(), new_value);
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

//! Tests setting value to enum Parameter from a JSON command
TEST_F(ParameterTest, EnumParameterSetValue)
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
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    std::string    new_value = "field2";   // enums are serialized as strings
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    component.flipBufferState();

    EXPECT_EQ(parameter.value(), TestEnum::field2);   // tests explicit access
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

//! Tests setting value to array of double Parameter from a JSON command
TEST_F(ParameterTest, DoubleArrayParameterSetValue)
{
    MockComponent                    component;   // component to attach parameters to
    const std::string                parameter_name = "double_array";
    Parameter<std::array<double, 3>> parameter(component, parameter_name, -5, 5);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    std::array<double, 3> new_value = {0.1, 1.2, 2.3};
    nlohmann::json        command   = {{"value", new_value}};
    auto                  output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);
    component.flipBufferState();

    size_t counter = 0;
    // tests begin() and end() methods of Parameter
    for (auto const& element : parameter.value())
    {
        EXPECT_EQ(element, new_value[counter++]);
    }
    // tests overloaded operator[]
    EXPECT_EQ(parameter[2], new_value[2]);
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

// ************************************************************
// Tests of attempting to set an invalid value via JSON

//! Tests setting out-of-limits value to double Parameter from a JSON command
TEST_F(ParameterTest, DoubleParameterSetInvalidValue)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "double";
    Parameter<double> parameter(component, parameter_name, -1, 5);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    double         new_value = 10;   // out of limits
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), true);   // there is a warning message
    EXPECT_EQ(fmt::format("{}", output.value()), "Warning: Provided value: 10 is outside the limits: -1, 5.\n");
    component.flipBufferState();

    EXPECT_NE(parameter.value(), new_value);
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

//! Tests setting value to enum Parameter from a JSON command
TEST_F(ParameterTest, EnumParameterSetInvalidValue)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "enum";
    enum class TestEnum
    {
        field1,
        field2
    };
    Parameter<TestEnum> parameter(component, parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    std::string    new_value = "field5";   // enums are serialized as strings
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), true);   // there is a warning message
    EXPECT_EQ(
        fmt::format("{}", output.value()), "Warning: The provided enum value is not one of the allowed values.\n"
    );
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

//! Tests setting out-of-limits value to array of double Parameter from a JSON command
TEST_F(ParameterTest, DoubleArrayParameterSetInvalidValue)
{
    MockComponent                    component;   // component to attach parameters to
    const std::string                parameter_name = "double_array";
    Parameter<std::array<double, 3>> parameter(component, parameter_name, -1, 1);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    std::array<double, 3> new_value = {0.1, 1.2, 2.3};
    nlohmann::json        command   = {{"value", new_value}};
    auto                  output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), true);
    component.flipBufferState();

    size_t counter = 0;
    // implicitly tests begin() and end() methods of Parameter
    for (auto const& element : parameter.value())
    {
        EXPECT_NE(element, new_value[counter++]);
    }
    // tests overloaded operator[]
    EXPECT_NE(parameter[2], new_value[2]);
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

//! Tests setting wrong type value to double Parameter from a JSON command
TEST_F(ParameterTest, ParameterSetInvalidTypeValue)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "double";
    Parameter<double> parameter(component, parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    std::string    new_value = "invalid";
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    ASSERT_EQ(output.has_value(), true);   // there is a warning message
    EXPECT_EQ(output.value().warning_str, "[json.exception.type_error.302] type must be number, but is string.\n");
    EXPECT_TRUE(parameter.isInitialized());
    EXPECT_TRUE(component.parametersInitialized());
}

// ************************************************************

//! Tests accessing out-of-bounds value of a array of ints Parameter
TEST_F(ParameterTest, IntArrayParameterOutOfBounds)
{
    MockComponent                 component;   // component to attach parameters to
    const std::string             parameter_name = "int_array";
    Parameter<std::array<int, 5>> parameter(component, parameter_name);
    EXPECT_FALSE(parameter.isInitialized());
    EXPECT_FALSE(component.parametersInitialized());

    // tests the correct exception is thrown from the overloaded operator[]
    ASSERT_THROW(parameter[6], std::out_of_range);
}

// ************************************************************
// Tests of methods synchronising buffers after a value has been set

//! Tests synchronising buffers of uint64 Parameter when value has been set
TEST_F(ParameterTest, IntParameterSynchronizeBuffers)
{
    MockComponent       component;   // component to attach parameters to
    const std::string   parameter_name = "int";
    Parameter<uint64_t> parameter(component, parameter_name);

    uint64_t       new_value = 120500;
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);

    component.flipBufferState();
    parameter.syncInactiveBuffer();   // synchronises background buffer with read

    EXPECT_EQ(parameter.value(), new_value);   // tests explicit access
    EXPECT_EQ(parameter, new_value);           // tests overloaded operator()
}

//! Tests synchronising buffers of float Parameter when value has been set
TEST_F(ParameterTest, FloatParameterSynchronizeBuffers)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "float";
    Parameter<float>  parameter(component, parameter_name);

    float          new_value = (float)3.1415;
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);

    component.flipBufferState();
    parameter.syncInactiveBuffer();

    EXPECT_EQ(parameter.value(), new_value);   // tests explicit access
    EXPECT_EQ(parameter, new_value);           // tests overloaded operator()
}

//! Tests synchronising buffers of std::string Parameter when value has been set
TEST_F(ParameterTest, StringParameterSynchronizeBuffers)
{
    MockComponent          component;   // component to attach parameters to
    const std::string      parameter_name = "string";
    Parameter<std::string> parameter(component, parameter_name);

    std::string    new_value = "new_text_parameter";
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);

    component.flipBufferState();
    parameter.syncInactiveBuffer();

    EXPECT_EQ(parameter.value(), new_value);   // tests explicit access
}

//! Tests synchronising buffers of array of double Parameter when value has been set
TEST_F(ParameterTest, DoubleArrayParameterSynchronizeBuffers)
{
    MockComponent                    component;   // component to attach parameters to
    const std::string                parameter_name = "array_double";
    Parameter<std::array<double, 3>> parameter(component, parameter_name);

    std::array<double, 4> new_value = {1.1, 2.2, 3.3, 4.4};
    nlohmann::json        command   = {{"value", new_value}};
    auto                  output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);

    component.flipBufferState();
    parameter.syncInactiveBuffer();

    size_t counter = 0;
    // implicitly tests begin() and end() methods of Parameter
    for (auto const& element : parameter.value())
    {
        EXPECT_EQ(element, new_value[counter++]);
    }
    // tests overloaded operator[]
    EXPECT_EQ(parameter[2], new_value[2]);
}

//! Tests synchronising buffers of array of std::string Parameter when value has been set
TEST_F(ParameterTest, StringArrayParameterSynchronizeBuffers)
{
    MockComponent                         component;   // component to attach parameters to
    const std::string                     parameter_name = "array_double";
    Parameter<std::array<std::string, 3>> parameter(component, parameter_name);

    std::array<std::string, 3> new_value = {"text1", "text2", "text3"};
    nlohmann::json             command   = {{"value", new_value}};
    auto                       output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);

    component.flipBufferState();
    parameter.syncInactiveBuffer();

    size_t counter = 0;
    // implicitly tests begin() and end() methods of Parameter
    for (auto const& element : parameter.value())
    {
        EXPECT_EQ(element, new_value[counter++]);
    }
    // tests overloaded operator[]
    EXPECT_EQ(parameter.value()[2], new_value[2]);
}

//! Tests synchronising buffers of enum Parameter when value has been set
TEST_F(ParameterTest, EnumParameterSynchronizeBuffers)
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

    std::string    new_value = "field2";   // enums are serialized as strings
    nlohmann::json command   = {{"value", new_value}};
    auto           output    = parameter.setJsonValue(command["value"]);
    EXPECT_EQ(output.has_value(), false);

    component.flipBufferState();
    parameter.syncInactiveBuffer();

    EXPECT_EQ(parameter.value(), TestEnum::field2);   // tests explicit access
}

//! Tests providing a number of commands and synchronising buffers each time
TEST_F(ParameterTest, FloatParameterSendManyCommands)
{
    MockComponent     component;   // component to attach parameters to
    const std::string parameter_name = "float";
    Parameter<float>  parameter(component, parameter_name);

    for (size_t command_no = 0; command_no < 5; command_no++)
    {
        float          new_value = static_cast<float>(command_no) * (float)3.14;   // enums are serialized as strings
        nlohmann::json command   = {{"value", new_value}};
        auto           output    = parameter.setJsonValue(command["value"]);
        EXPECT_EQ(output.has_value(), false);

        component.flipBufferState();
        parameter.syncInactiveBuffer();

        EXPECT_EQ(parameter.value(), new_value);   // tests explicit access
        EXPECT_EQ(parameter, new_value);           // tests implicit access
    }
}

// ************************************************************
// Test of operations on Parameter with value set via JSON

//! Tests spaceship operator of a double Parameter with value set via JSON command
TEST_F(ParameterTest, DoubleParameterValueOperations)
{
    MockComponent     component;   // component to attach parameters to
    Parameter<double> lhs(component, "lhs");
    Parameter<double> rhs(component, "rhs");

    double         new_lhs     = 3.14159;
    nlohmann::json command_lhs = {{"value", new_lhs}};
    auto           output_lhs  = lhs.setJsonValue(command_lhs["value"]);
    EXPECT_EQ(output_lhs.has_value(), false);

    double         new_rhs     = 2.71828;
    nlohmann::json command_rhs = {{"name", "mockType.mockName.rhs"}, {"value", new_rhs}};
    auto           output_rhs  = rhs.setJsonValue(command_rhs["value"]);
    EXPECT_EQ(output_rhs.has_value(), false);

    component.flipBufferState();
    lhs.syncInactiveBuffer();   // synchronises background buffer with read
    rhs.syncInactiveBuffer();

    ASSERT_EQ(lhs.value(), new_lhs);
    ASSERT_EQ(rhs.value(), new_rhs);

    EXPECT_EQ(lhs == lhs, true);
    EXPECT_EQ(rhs == rhs, true);
    EXPECT_EQ(lhs != rhs, true);
    EXPECT_EQ(lhs == rhs, false);
    EXPECT_EQ(lhs > rhs, true);
    EXPECT_EQ(lhs >= rhs, true);
    EXPECT_EQ(lhs < rhs, false);
    EXPECT_EQ(lhs <= rhs, false);
    EXPECT_EQ(lhs < (rhs * 2), true);
    EXPECT_EQ((lhs - 1) < rhs, true);
}
