//! @file
//! @brief File with unit tests of CosLookupTable component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "cosLookupTable.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;

class CosLookupTableTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of integral type CosLookupTable component
TEST_F(CosLookupTableTest, CosLookupTable)
{
    RootComponent  root;
    std::string    name = "table";
    CosLookupTable table(name, root, 2);
    EXPECT_EQ(table.getName(), name);

    auto serialized = table.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "CosLookupTable");
    EXPECT_EQ(
        serialized["components"].dump(),
        "[{\"name\":\"data\",\"type\":\"LookupTable\",\"parameters\":[],\"components\":[]}]"
    );
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests CosLookupTable component interpolating a couple of points
TEST_F(CosLookupTableTest, CosLookupTableInterpolation)
{
    RootComponent  root;
    std::string    name = "table";
    CosLookupTable table(name, root, 10000);

    EXPECT_NEAR(table.interpolate(0.0), cos(0.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI / 2.0), cos(M_PI / 2.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI), cos(M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(4.0 / 3.0 * M_PI), cos(4.0 / 3.0 * M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(2.0 * M_PI - 0.01), cos(2.0 * M_PI - 0.01), 1e-6);
}

//! Tests CosLookupTable component interpolating a couple of points that fall out of 0 - 2pi range
TEST_F(CosLookupTableTest, CosLookupTableInterpolationOutOfBounds)
{
    RootComponent  root;
    std::string    name = "table";
    CosLookupTable table(name, root, 10000);

    const double epsilon = 1e-2;

    EXPECT_NEAR(table.interpolate(-M_PI), cos(-M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(-M_PI / 2.0), cos(-M_PI / 2.0), 1e-3);
    EXPECT_NEAR(table.interpolate(-M_PI), cos(-M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(-4.0 / 3.0 * M_PI), cos(-4.0 / 3.0 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(-2.0 * M_PI + epsilon), cos(-2.0 * M_PI + epsilon), 1e-3);

    EXPECT_NEAR(table.interpolate(5.5 * M_PI), cos(5.5 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(15.3 * M_PI / 2.0), cos(15.3 * M_PI / 2.0), 1e-3);
    EXPECT_NEAR(table.interpolate(7.939 * M_PI), cos(7.939 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(11.5 * 4.0 / 3.0 * M_PI), cos(11.5 * 4.0 / 3.0 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(123 * 2.0 * M_PI), cos(123 * 2.0 * M_PI), 1e-3);
}

//! Tests CosLookupTable consistency of interface
TEST_F(CosLookupTableTest, CosLookupTableConsistency)
{
    RootComponent  root;
    std::string    name = "table";
    CosLookupTable table(name, root, 10000);

    EXPECT_NEAR(table.interpolate(0.0), table(0.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI / 2.0), table(M_PI / 2.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI), table(M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(4.0 / 3.0 * M_PI), table(4.0 / 3.0 * M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(2.0 * M_PI - 0.01), table(2.0 * M_PI - 0.01), 1e-6);
}