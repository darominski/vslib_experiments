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
    std::vector<std::pair<int32_t, int32_t>> data_table{std::make_pair(0, 0)};
    LookupTable<int32_t>                     table(name, nullptr, std::move(data_table));
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
    std::vector<std::pair<double, double>> data_table{std::make_pair(0.0, 0.0)};
    LookupTable<double>                    table(name, nullptr, std::move(data_table));
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

//! Tests LookupTable component with a meaningful integer table and then interpolating with trivial case of
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

//! Tests LookupTable component with a meaningful integer table and then interpolating with trivial case of
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

//! Tests LookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(LookupTableTest, LookupTableDoubleProvidedData)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
}

//! Tests LookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(LookupTableTest, LookupTableDoubleNegativeAxis)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

    EXPECT_NEAR(table.interpolate(-3.0), 3.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-1.0), 1.3, 1e-15);

    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(-1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-3.0), 3.3, 1e-15);
}

//! Tests LookupTable component with a meaningful double table and then interpolating with a more realistic case of
//! interpolation input being somewhere between the data points
TEST_F(LookupTableTest, LookupTableDoubleInterpolateBetweenPoints)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

    EXPECT_NEAR(table.interpolate(-2.5), 0.5 * (3.3 + 2.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-0.5), 0.5 * (1.3 + 0.3), 1e-15);

    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(-0.5), 0.5 * (1.3 + 0.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-2.5), 0.5 * (3.3 + 2.3), 1e-15);
}


//! Tests LookupTable provides the expected saturation behaviour the input is below the provided data limits
TEST_F(LookupTableTest, LookupTableIntInterpolateBelowLimitsConsistency)
{
    std::string                      name = "table";
    std::vector<std::pair<int, int>> data{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, nullptr, std::move(data));

    EXPECT_EQ(table.interpolate(-4), 3);
    EXPECT_EQ(table.interpolate(-100), 3);
    EXPECT_EQ(table.interpolate(-15), table.interpolate(-1000));
}

//! Tests LookupTable provides the expected saturation behaviour the input is above the provided data limits
TEST_F(LookupTableTest, LookupTableIntInterpolateAboveLimits)
{
    std::string                      name = "table";
    std::vector<std::pair<int, int>> data{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, nullptr, std::move(data));

    EXPECT_EQ(table.interpolate(4), 0);
}

//! Tests LookupTable provides the expected output regardless whether the switch for random access is true or not
TEST_F(LookupTableTest, LookupTableIntRandomAccessConsistency)
{
    std::string                         name = "table";
    std::vector<std::pair<double, int>> data{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double, int>            table(name, nullptr, std::move(data));

    EXPECT_EQ(table.interpolate(-3.5), table.interpolate(-3.5, true));
    EXPECT_EQ(table.interpolate(-3), table.interpolate(-3, true));
    EXPECT_EQ(table.interpolate(-2.5), table.interpolate(-2.5, true));
    EXPECT_EQ(table.interpolate(-2), table.interpolate(-2, true));
    EXPECT_EQ(table.interpolate(-1.5), table.interpolate(-1.5, true));
    EXPECT_EQ(table.interpolate(-1), table.interpolate(-1, true));
    EXPECT_EQ(table.interpolate(-0.5), table.interpolate(-0.5, true));
    EXPECT_EQ(table.interpolate(0), table.interpolate(0, true));
}

//! Tests LookupTable provides the expected output regardless whether the switch for random access is true or not
TEST_F(LookupTableTest, LookupTableDoubleRandomAccessConsistency)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> data{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double>                    table(name, nullptr, std::move(data));

    EXPECT_EQ(table.interpolate(-3.5), table.interpolate(-3.5, true));
    EXPECT_EQ(table.interpolate(-3), table.interpolate(-3, true));
    EXPECT_EQ(table.interpolate(-2.5), table.interpolate(-2.5, true));
    EXPECT_EQ(table.interpolate(-2), table.interpolate(-2, true));
    EXPECT_EQ(table.interpolate(-1.5), table.interpolate(-1.5, true));
    EXPECT_EQ(table.interpolate(-1), table.interpolate(-1, true));
    EXPECT_EQ(table.interpolate(-0.5), table.interpolate(-0.5, true));
    EXPECT_EQ(table.interpolate(0), table.interpolate(0, true));
}