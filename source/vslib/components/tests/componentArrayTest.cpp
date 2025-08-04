//! @file
//! @brief File with unit tests of ComponentRegistry class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.hpp"
#include "componentArray.hpp"
#include "json/json.hpp"
#include "mockRoot.hpp"

using namespace vslib;

class ComponentArrayTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

class Derived : public Component
{
  public:
    Derived(std::string_view name, Component& parent)
        : Component("Derived", name, parent)
    {
    }
};

//! Checks that a basic component array holding simple derived component class can be created and is correctly
//! serialized
TEST_F(ComponentArrayTest, BasicArray)
{
    MockRoot                              root;
    const std::string                     component_name = "array";
    constexpr size_t                      array_length   = 3;
    ComponentArray<Derived, array_length> component(component_name, root);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), std::string("root.") + component_name);
    EXPECT_EQ(component.getParameters().size(), 0);

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
    MockRoot                              root;
    const std::string                     component_name = "array";
    constexpr size_t                      array_length   = 4;
    ComponentArray<Derived, array_length> component(component_name, root);
    const std::string                     component_type = "ComponentArray";
    const std::string                     held_type_name = "Derived";

    size_t counter = 0;
    for (const auto& element : component)   // tests begin() and end() operators
    {
        std::string element_name = std::string("array[") + std::to_string(counter++) + "]";
        EXPECT_EQ(element.getName(), element_name);
        EXPECT_EQ(element.getFullName(), std::string("root.") + component_name + "." + element_name);
        EXPECT_EQ(element.getParameters().size(), 0);
    }
    // tests operator[] overload:
    EXPECT_EQ(component[2].getName(), "array[2]");
}

//! Checks that ComponentArray can hold a ComponentArray
TEST_F(ComponentArrayTest, HierarchicalArrayTest)
{
    MockRoot root;

    const std::string                                                               component_name     = "array";
    constexpr size_t                                                                inner_array_length = 4;
    constexpr size_t                                                                outer_array_length = 2;
    ComponentArray<ComponentArray<Derived, inner_array_length>, outer_array_length> component(component_name, root);

    EXPECT_EQ(component.getName(), component_name);
    EXPECT_EQ(component.getFullName(), std::string("root.") + component_name);
    EXPECT_EQ(component.getParameters().size(), 0);

    auto serialized_component = component.serialize();
    EXPECT_EQ(serialized_component["name"], component_name);
    EXPECT_EQ(serialized_component["type"], "ComponentArray");
    EXPECT_EQ(serialized_component["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"].size(), outer_array_length);
    EXPECT_EQ(serialized_component["components"][0]["type"], "ComponentArray");
    EXPECT_EQ(serialized_component["components"][0]["name"], "array[0]");
    EXPECT_EQ(serialized_component["components"][0]["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"][0]["components"].size(), inner_array_length);
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["type"], "Derived");
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["name"], "array[0][0]");
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["parameters"], nlohmann::json::array());
    EXPECT_EQ(serialized_component["components"][0]["components"][0]["components"].size(), 0);
}
