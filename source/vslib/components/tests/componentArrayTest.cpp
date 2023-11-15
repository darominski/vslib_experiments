//! @file
//! @brief File with unit tests of ComponentRegistry class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.h"
#include "componentArray.h"
#include "componentRegistry.h"
#include "json/json.hpp"

using namespace vslib;

class ComponentArrayTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // cleans up the registry so every test starts anew, otherwise
        // the registry would persist between tests
        ComponentRegistry& registry = ComponentRegistry::instance();
        registry.clearRegistry();
    }

    void TearDown() override
    {
    }
};

class Derived : public Component
{
  public:
    Derived(std::string_view name, Component* parent)
        : Component("Derived", name, parent)
    {
    }
};

//! Checks that a basic component array holding simple derived component class can be created and is correctly
//! serialized
TEST_F(ComponentArrayTest, BasicArray)
{
    const std::string          component_name = "array";
    ComponentArray<Derived, 3> component(component_name, nullptr);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), std::string("ComponentArray.") + component_name);
    EXPECT_EQ(component.getParameters().size(), 0);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(component.getFullName()), registry.getComponents().end());

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], "ComponentArray");
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"].size(), 3);
    EXPECT_EQ(serialized_component["components"][0]["type"], "Derived");
    EXPECT_EQ(serialized_component["components"][0]["name"], "array[0]");
    EXPECT_EQ(serialized_component["components"][0]["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"][0]["components"], nlohmann::json::array());
}

//! Checks that a basic component array can be interacted with as if it is an array
TEST_F(ComponentArrayTest, BasicArrayInteractions)
{
    const std::string          component_name = "array";
    ComponentArray<Derived, 3> component(component_name, nullptr);
    const std::string          component_type = "ComponentArray";
    const std::string          held_type_name = "Derived";

    size_t counter = 0;
    for (const auto& element : component)   // tests begin() and end() operators
    {
        std::string element_name = std::string("array[") + std::to_string(counter++) + "]";
        EXPECT_EQ(element.getName(), element_name);
        EXPECT_EQ(
            element.getFullName(), component_type + "." + component_name + "." + held_type_name + "." + element_name
        );
        EXPECT_EQ(element.getParameters().size(), 0);
    }
    // tests operator[] overload:
    EXPECT_EQ(component[2].getName(), "array[2]");
}

//! Checks that ComponentArray can hold a ComponentArray
TEST_F(ComponentArrayTest, HierarchicalArrayTest)
{
    const std::string                             component_name = "array";
    ComponentArray<ComponentArray<Derived, 3>, 1> component(component_name, nullptr);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), std::string("ComponentArray.") + component_name);
    EXPECT_EQ(component.getParameters().size(), 0);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(component.getFullName()), registry.getComponents().end());

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], "ComponentArray");
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"].size(), 1);
    EXPECT_EQ(serialized_component["components"][0]["type"], "ComponentArray");
    EXPECT_EQ(serialized_component["components"][0]["name"], "array[0]");
    EXPECT_EQ(serialized_component["components"][0]["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"][0]["components"].size(), 3);
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["type"], "Derived");
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["name"], "array[0][0]");
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["components"].size(), 0);
}
