//! @file
//! @brief File with unit tests of ParameterRegistry class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.hpp"
#include "json/json.hpp"
#include "parameter.hpp"
#include "parameterRegistry.hpp"
#include "rootComponent.hpp"

using namespace vslib;

class ParameterRegistryTest : public ::testing::Test
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
            : Component("MockType", "mock_name", parent)
        {
        }
    };
}

//! Checks that an instance of the empty ParameterRegistry can be created
TEST_F(ParameterRegistryTest, EmptyInstance)
{
    ParameterRegistry& registry   = ParameterRegistry::instance();
    auto&              parameters = registry.getParameters();
    ASSERT_EQ(parameters.size(), 0);
}

//! Checks Parameter is correctly added to the registry when attached to a component
TEST_F(ParameterRegistryTest, AutomaticParameterRegistration)
{
    RootComponent     root;
    MockComponent     component(root);   // component to attach parameters to
    const std::string parameter_name = "parameter";
    Parameter<int>    parameter(component, parameter_name);
    const auto&       parameters = ParameterRegistry::instance().getParameters();
    ASSERT_EQ(parameters.size(), 1);
    const std::string registry_name = std::string("root.mock_name.") + parameter_name;
    EXPECT_NE(parameters.find(registry_name), parameters.end());
}

//! Checks explicit registration of a Parameter
TEST_F(ParameterRegistryTest, ExplicitAddToRegistry)
{
    RootComponent     root;
    MockComponent     component(root);   // component to attach parameters to
    const std::string parameter_name = "parameter";
    Parameter<int>    parameter(component, parameter_name);

    auto& registry   = ParameterRegistry::instance();
    auto& parameters = registry.getParameters();
    ASSERT_EQ(parameters.size(), 1);
    const std::string registry_name = std::string("root.mock_name.") + parameter_name;
    EXPECT_NE(parameters.find(registry_name), parameters.end());

    const std::string new_parameter_name = "root.new_name";
    registry.addToRegistry(new_parameter_name, parameter);
    EXPECT_EQ(parameters.size(), 2);
    EXPECT_NE(parameters.find(registry_name), parameters.end());
    EXPECT_NE(parameters.find(new_parameter_name), parameters.end());
}

//! Checks automatic registration of a number of Parameters of different types
TEST_F(ParameterRegistryTest, AutomaticMultipleParameterRegistration)
{
    RootComponent          root;
    MockComponent          component(root);   // component to attach parameters to
    const std::string      int_name    = "int";
    const std::string      double_name = "double";
    const std::string      bool_name   = "bool";
    const std::string      string_name = "string";
    const std::string      enum_name   = "enum";
    Parameter<int>         int_parameter(component, int_name);
    Parameter<double>      double_parameter(component, double_name);
    Parameter<bool>        bool_parameter(component, bool_name);
    Parameter<std::string> string_parameter(component, string_name);
    enum class TestEnum
    {
        field1,
        field2
    };
    Parameter<TestEnum> enum_parameter(component, enum_name);

    const auto& parameters = ParameterRegistry::instance().getParameters();
    ASSERT_EQ(parameters.size(), 5);
    EXPECT_NE(parameters.find(std::string("root.mock_name.") + int_name), parameters.end());
    EXPECT_NE(parameters.find(std::string("root.mock_name.") + double_name), parameters.end());
    EXPECT_NE(parameters.find(std::string("root.mock_name.") + bool_name), parameters.end());
    EXPECT_NE(parameters.find(std::string("root.mock_name.") + string_name), parameters.end());
    EXPECT_NE(parameters.find(std::string("root.mock_name.") + enum_name), parameters.end());
}

//! Checks that correct exception is thrown when the name already exists in the registry
TEST_F(ParameterRegistryTest, NameExistsError)
{
    RootComponent  root;
    MockComponent  component(root);   // component to attach parameters to
    Parameter<int> parameter(component, "name");

    auto& registry = ParameterRegistry::instance();
    EXPECT_THROW(registry.addToRegistry("root.mock_name.name", parameter), std::runtime_error);
}

//! Checks that correct exception is thrown when the name of a Parameter does not follow the snake_case formatting
TEST_F(ParameterRegistryTest, NameInvalidError)
{
    RootComponent  root;
    MockComponent  component(root);   // component to attach parameters to
    Parameter<int> parameter(component, "name");

    auto& registry = ParameterRegistry::instance();
    // capital letters:
    EXPECT_THROW(registry.addToRegistry("root.mock_name.Name", parameter), std::runtime_error);
    // empty spaces:
    EXPECT_THROW(registry.addToRegistry("root.mock_name.name second", parameter), std::runtime_error);
    // other characters than alphanumeric:
    EXPECT_THROW(registry.addToRegistry("root.mock_name.offending!char$", parameter), std::runtime_error);
}
