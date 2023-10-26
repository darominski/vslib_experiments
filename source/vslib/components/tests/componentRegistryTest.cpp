//! @file
//! @brief File with unit tests of ComponentRegistry class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.h"
#include "componentRegistry.h"
#include "json/json.hpp"

using namespace vslib::components;

class ComponentRegistryTest : public ::testing::Test
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

//! Checks that an instance of the empty ComponentRegistry can be created
TEST_F(ComponentRegistryTest, EmptyInstance)
{
    ComponentRegistry& component_registry = ComponentRegistry::instance();
    auto&              components         = component_registry.getComponents();
    ASSERT_EQ(components.size(), 0);
}

//! Checks that a manifest of the empty ComponentRegistry can be created
TEST_F(ComponentRegistryTest, EmptyInstanceManifest)
{
    ComponentRegistry& component_registry = ComponentRegistry::instance();
    EXPECT_EQ(component_registry.createManifest(), nlohmann::json::array());
}

//! Checks automatically adding a component to the ComponentRegistry
TEST_F(ComponentRegistryTest, AddComponentAutomatically)
{
    ComponentRegistry&         registry       = ComponentRegistry::instance();
    constexpr std::string_view component_type = "TestComponentType";
    constexpr std::string_view component_name = "TestComponent";
    Component                  component(component_type, component_name, nullptr);
    const auto                 name_in_registry = component.getFullName();

    // Verify that the component has been added to the registry
    auto& components = registry.getComponents();
    ASSERT_EQ(components.size(), 1);
    ASSERT_NE(components.find(name_in_registry), components.end());
}

//! Checks the manifest created for a component automatically registered in the ComponentRegistry
TEST_F(ComponentRegistryTest, AutomaticComponentManifest)
{
    ComponentRegistry&         registry       = ComponentRegistry::instance();
    constexpr std::string_view component_type = "TestComponentType";
    constexpr std::string_view component_name = "TestComponent";
    Component                  component(component_type, component_name, nullptr);

    auto& components = registry.getComponents();
    ASSERT_EQ(components.size(), 1);
    nlohmann::json expected_manifest
        = {{{"name", "TestComponent"},
            {"type", "TestComponentType"},
            {"parameters", nlohmann::json::array()},
            {"components", nlohmann::json::array()}}};
    nlohmann::json created_manifest = registry.createManifest();
    EXPECT_EQ(created_manifest, expected_manifest);
}

//! Checks explicitly adding a component to the ComponentRegistry
TEST_F(ComponentRegistryTest, AddComponentExplicitly)
{
    ComponentRegistry& registry = ComponentRegistry::instance();

    constexpr std::string_view component_type = "TestComponentType";
    constexpr std::string_view component_name = "TestComponent";
    Component                  component(component_type, component_name, nullptr);
    const std::string          name_in_registry = component.getFullName();

    registry.addToRegistry("TestComponent2", component);
    const std::string name_in_registry_2 = std::string(component_type) + std::string("TestComponent2");

    // Verify that the component has been added to the registry
    auto& components = registry.getComponents();
    ASSERT_EQ(components.size(), 2);
    EXPECT_NE(components.find(name_in_registry), components.end());
    EXPECT_NE(components.find("TestComponent2"), components.end());
}

//! Checks adding a number of components to the ComponentRegistry
TEST_F(ComponentRegistryTest, AddManyComponents)
{
    ComponentRegistry& registry = ComponentRegistry::instance();

    const std::string component_type = "TestComponentType";
    Component         component1(component_type, "Component1", nullptr);
    Component         component2(component_type, "Component2", nullptr);
    Component         component3(component_type, "Component3", nullptr);

    // Verify that the component has been added to the registry
    auto& components = registry.getComponents();
    ASSERT_EQ(components.size(), 3);
    EXPECT_NE(components.find(component_type + std::string(".Component1")), components.end());
    EXPECT_NE(components.find(component_type + std::string(".Component2")), components.end());
    EXPECT_NE(components.find(component_type + std::string(".Component3")), components.end());
}

//! Checks exception is thrown when two components with identical names are declared
TEST_F(ComponentRegistryTest, AddComponentsWithSameName)
{
    ComponentRegistry& registry = ComponentRegistry::instance();

    const std::string component_type = "TestComponentType";
    Component         component1(component_type, "Component1", nullptr);
    Component         component2(component_type, "Component2", nullptr);

    // Try adding "another" component with the already existing name
    ASSERT_THROW(registry.addToRegistry(component_type + std::string(".Component1"), component2), std::runtime_error);
}
