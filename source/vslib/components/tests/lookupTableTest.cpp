//! @file
//! @brief File with unit tests of LookupTable component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "componentRegistry.h"
#include "lookupTable.h"
#include "staticJson.h"

using namespace vslib;

class LookupTableTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        ComponentRegistry& component_registry = ComponentRegistry::instance();
        component_registry.clearRegistry();
    }
};


//! Tests default construction of integral type LookupTable component
TEST_F(LookupTableTest, LookupTableIntDefault)
{
    std::string                              name = "table";
    std::vector<std::pair<int32_t, int32_t>> empty_vector;
    LookupTable<int32_t>                     table(name, nullptr, std::move(empty_vector));
    EXPECT_EQ(table.getName(), name);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(table.getFullName()), registry.getComponents().end());

    auto serialized = table.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LookupTable");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests default construction of double type LookupTable component
TEST_F(LookupTableTest, LookupTableDoubleDefault)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> empty_vector;
    LookupTable<double>                    table(name, nullptr, std::move(empty_vector));
    EXPECT_EQ(table.getName(), name);

    ComponentRegistry& registry = ComponentRegistry::instance();
    EXPECT_EQ(registry.getComponents().size(), 1);
    EXPECT_NE(registry.getComponents().find(table.getFullName()), registry.getComponents().end());

    auto serialized = table.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LookupTable");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests setting up LookupTable component with a meaningful integer table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(LookupTableTest, LookupTableIntInterpolateProvidedData)
{
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    LookupTable<int>                 table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(0), 0);
    EXPECT_EQ(table.interpolate(1), 1);
    EXPECT_EQ(table.interpolate(2), 2);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_EQ(table.interpolate(2), 2);
    EXPECT_EQ(table.interpolate(1), 1);
    EXPECT_EQ(table.interpolate(0), 0);
}

//! Tests setting up LookupTable component with a meaningful integer table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(LookupTableTest, LookupTableIntInterpolateProvidedDataNegativeAxis)
{
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(0), 0);
    EXPECT_EQ(table.interpolate(-1), 1);
    EXPECT_EQ(table.interpolate(-2), 2);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_EQ(table.interpolate(-2), 2);
    EXPECT_EQ(table.interpolate(-1), 1);
    EXPECT_EQ(table.interpolate(0), 0);
}

//! Tests setting up LookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(LookupTableTest, LookupTableDoubleProvidedData)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(0.0), 0.3);
    EXPECT_EQ(table.interpolate(1.0), 1.3);
    EXPECT_EQ(table.interpolate(2.0), 2.3);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_EQ(table.interpolate(2.0), 2.3);
    EXPECT_EQ(table.interpolate(1.0), 1.3);
    EXPECT_EQ(table.interpolate(0.0), 0.3);
}

//! Tests setting up LookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(LookupTableTest, LookupTableDoubleProvidedDataNegativeAxis)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(-3.0), 3.3);
    EXPECT_EQ(table.interpolate(-2.0), 2.3);
    EXPECT_EQ(table.interpolate(-1.0), 1.3);

    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_EQ(table.interpolate(-1.0), 1.3);
    EXPECT_EQ(table.interpolate(-2.0), 2.3);
    EXPECT_EQ(table.interpolate(-3.0), 3.3);
}

//! Tests that LookupTable raises the expected warning with a meaningful integer table and then interpolating with
//! trivial case of hitting the provided points
TEST_F(LookupTableTest, LookupTableIntInterpolateProvidedDataNegativeAxis)
{
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(0), 0);
    EXPECT_EQ(table.interpolate(-1), 1);
    EXPECT_EQ(table.interpolate(-2), 2);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_EQ(table.interpolate(-2), 2);
    EXPECT_EQ(table.interpolate(-1), 1);
    EXPECT_EQ(table.interpolate(0), 0);
}