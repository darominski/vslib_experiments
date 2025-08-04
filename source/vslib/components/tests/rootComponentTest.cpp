//! @file
//! @brief File with unit tests of the RootComponent class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "json/json.hpp"
#include "parameter.hpp"
#include "parameterRegistry.hpp"
#include "rootComponent.hpp"

using namespace vslib;

class RootComponentTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // cleans up the registry so every test starts anew, otherwise
        // it would persist between tests
        ParameterRegistry& parameter_registry = ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    void TearDown() override
    {
    }
};

class DerivedComponent : public Component
{
  public:
    DerivedComponent(std::string_view type, std::string_view name, RootComponent& parent)
        : Component(type, name, parent)
    {
    }
};

class DerivedComponentIntParameter : public Component
{
  public:
    DerivedComponentIntParameter(std::string_view type, std::string_view name, RootComponent& parent)
        : Component(type, name, parent),
          parameter(*this, "int")
    {
    }

    Parameter<uint32_t> parameter;
};

//! Checks that a RootComponent can be created, and then be serialized
TEST_F(RootComponentTest, StandAloneRootComponent)
{
    MockRoot root;

    EXPECT_EQ(root.getName(), "root");
    EXPECT_EQ(root.getFullName(), "root");
    EXPECT_EQ(root.getParameters().size(), 0);
    EXPECT_EQ(root.getChildren().size(), 0);

    EXPECT_TRUE(root.parametersInitialized());

    auto serialized_component = root.serialize();
    EXPECT_EQ(serialized_component["name"], "root");
    EXPECT_EQ(serialized_component["type"], "Root");
    EXPECT_EQ(serialized_component["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
}

//! Checks that a RootComponent can be used as a parent of a Component
TEST_F(RootComponentTest, DerivedComponent)
{
    MockRoot          root;
    const std::string component_type = "type";
    const std::string component_name = "name";
    DerivedComponent  component(component_type, component_name, root);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), std::string(root.getFullName()) + "." + component_name);
    EXPECT_EQ(component.getParameters().size(), 0);

    EXPECT_TRUE(component.parametersInitialized());

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], component_type);
    EXPECT_EQ(serialized_component["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
}

//! Checks that a hierarchical component with Component as the parent can be created,
//! and is correctly registered and serialized
TEST_F(RootComponentTest, HierarchicalComponent)
{
    MockRoot          root;
    const std::string parent_type = "type";
    const std::string parent_name = "name";
    Component         parent(parent_type, parent_name, root);

    const std::string child_type = "child_type";
    const std::string child_name = "child_name";
    Component         child(child_type, child_name, parent);

    EXPECT_EQ(child.getName(), child_name);
    EXPECT_EQ(child.getFullName(), std::string(root.getFullName()) + "." + parent_name + "." + child_name);
    EXPECT_EQ(child.getParameters().size(), 0);

    EXPECT_TRUE(child.parametersInitialized());

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
TEST_F(RootComponentTest, DerivedComponentIntParameter)
{
    MockRoot                     root;
    const std::string            component_type = "type";
    const std::string            component_name = "name";
    DerivedComponentIntParameter component(component_type, component_name, root);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), std::string(root.getFullName()) + "." + component_name);
    EXPECT_EQ(component.getParameters().size(), 1);
    EXPECT_FALSE(component.parametersInitialized());

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
TEST_F(RootComponentTest, DerivedComponentWithManyParameters)
{
    MockRoot                         root;
    const std::string                component_type = "type";
    const std::string                component_name = "name";
    DerivedComponentIntParameter     component(component_type, component_name, root);
    Parameter<double>                doubleParameter(component, "double");
    Parameter<bool>                  boolParameter(component, "bool");
    Parameter<std::array<double, 3>> arrayParameter(component, "array");

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), std::string(root.getFullName()) + "." + component_name);
    EXPECT_EQ(component.getParameters().size(), 4);

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
