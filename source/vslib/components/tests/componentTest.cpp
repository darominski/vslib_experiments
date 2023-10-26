//! @file
//! @brief File with unit tests of the Component class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.h"
#include "componentRegistry.h"
#include "json/json.hpp"
#include "parameter.h"
#include "parameterRegistry.h"

using namespace vslib;
using namespace vslib::components;

class ComponentTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // cleans up the registries so every test starts anew, otherwise
        // they would persist between tests
        ComponentRegistry& component_registry = ComponentRegistry::instance();
        component_registry.clearRegistry();
        parameters::ParameterRegistry& parameter_registry = parameters::ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    void TearDown() override
    {
    }
};

class DerivedComponent : public Component
{
  public:
    DerivedComponent(std::string_view type, std::string_view name, Component* parent)
        : Component(type, name, parent)
    {
    }
};

class DerivedComponentIntParameter : public Component
{
  public:
    DerivedComponentIntParameter(std::string_view type, std::string_view name, Component* parent)
        : Component(type, name, parent),
          parameter(*this, "int")
    {
    }

    parameters::Parameter<uint32_t> parameter;
};

//! Checks that a basic component of base class can be created, is registered, and can be serialized
TEST_F(ComponentTest, BasicComponent)
{
    const std::string component_type = "type";
    const std::string component_name = "name";
    Component         component(component_type, component_name, nullptr);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), component_type + "." + component_name);
    EXPECT_EQ(component.getParameters().size(), 0);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(component.getFullName()), registry.getComponents().end());

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], component_type);
    EXPECT_EQ(serialized_component["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
}

//! Checks that a basic derived component of base class can be created, and is registered correctly
TEST_F(ComponentTest, DerivedComponent)
{
    const std::string component_type = "type";
    const std::string component_name = "name";
    DerivedComponent  component(component_type, component_name, nullptr);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), component_type + "." + component_name);
    EXPECT_EQ(component.getParameters().size(), 0);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(component.getFullName()), registry.getComponents().end());

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], component_type);
    EXPECT_EQ(serialized_component["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
}

//! Checks that a hierarchical component with Component as the parent can be created,
//! and is correctly registered and serialized
TEST_F(ComponentTest, HierarchicalComponent)
{
    const std::string parent_type = "type";
    const std::string parent_name = "name";
    Component         parent(parent_type, parent_name, nullptr);

    const std::string child_type = "child_type";
    const std::string child_name = "child_name";
    Component         child(child_type, child_name, &parent);

    EXPECT_EQ(child.getName(), child_name);
    EXPECT_EQ(child.getFullName(), parent_type + "." + parent_name + "." + child_type + "." + child_name);
    EXPECT_EQ(child.getParameters().size(), 0);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(parent.getFullName()), registry.getComponents().end());

    auto serialized_component = parent.serialize();
    EXPECT_EQ(serialized_component["name"], parent_name);
    EXPECT_EQ(serialized_component["type"], parent_type);
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"].size(), 1);
    const auto& child_components = serialized_component["components"][0];
    EXPECT_EQ(child_components["name"], child_name);
    EXPECT_EQ(child_components["type"], child_type);
    EXPECT_EQ(child_components["parameters"], nlohmann::json::array());
    EXPECT_EQ(child_components["components"], nlohmann::json::array());
}

//! Checks derived component with a single integer parameter
TEST_F(ComponentTest, DerivedComponentIntParameter)
{
    const std::string            component_type = "type";
    const std::string            component_name = "name";
    DerivedComponentIntParameter component(component_type, component_name, nullptr);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), component_type + "." + component_name);
    EXPECT_EQ(component.getParameters().size(), 1);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(component.getFullName()), registry.getComponents().end());

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], component_type);
    EXPECT_EQ(serialized_component["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["parameters"][0]["length"], 1);
    EXPECT_EQ(serialized_component["parameters"][0]["name"], "int");
    EXPECT_EQ(serialized_component["parameters"][0]["type"], "UInt32");
    EXPECT_EQ(serialized_component["parameters"][0]["value"], nlohmann::json::object());
}

//! Checks derived component with many parameters of various types
TEST_F(ComponentTest, DerivedComponentWithManyParameters)
{
    const std::string                            component_type = "type";
    const std::string                            component_name = "name";
    DerivedComponentIntParameter                 component(component_type, component_name, nullptr);
    parameters::Parameter<double>                doubleParameter(component, "double");
    parameters::Parameter<bool>                  boolParameter(component, "bool");
    parameters::Parameter<std::array<double, 3>> arrayParameter(component, "array");

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), component_type + "." + component_name);
    EXPECT_EQ(component.getParameters().size(), 4);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(component.getFullName()), registry.getComponents().end());

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], component_type);
    EXPECT_EQ(serialized_component["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["parameters"][0]["length"], 1);
    EXPECT_EQ(serialized_component["parameters"][0]["name"], "int");
    EXPECT_EQ(serialized_component["parameters"][0]["type"], "UInt32");
    EXPECT_EQ(serialized_component["parameters"][0]["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_component["parameters"][1]["length"], 1);
    EXPECT_EQ(serialized_component["parameters"][1]["name"], "double");
    EXPECT_EQ(serialized_component["parameters"][1]["type"], "Float64");
    EXPECT_EQ(serialized_component["parameters"][1]["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_component["parameters"][2]["length"], 1);
    EXPECT_EQ(serialized_component["parameters"][2]["name"], "bool");
    EXPECT_EQ(serialized_component["parameters"][2]["type"], "Bool");
    EXPECT_EQ(serialized_component["parameters"][2]["value"], nlohmann::json::object());
    EXPECT_EQ(serialized_component["parameters"][3]["length"], 3);
    EXPECT_EQ(serialized_component["parameters"][3]["name"], "array");
    EXPECT_EQ(serialized_component["parameters"][3]["type"], "ArrayFloat64");
    EXPECT_EQ(serialized_component["parameters"][3]["value"], nlohmann::json::array());
}
