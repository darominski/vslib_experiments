//! @file
//! @brief File with unit tests of SinLookupTable component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "sinLookupTable.h"
#include "staticJson.h"

using namespace vslib;

class SinLookupTableTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of SinLookupTableTest component
TEST_F(SinLookupTableTest, SinLookupTable)
{
    std::string    name = "table";
    SinLookupTable table(name, nullptr, 2);
    EXPECT_EQ(table.getName(), name);

    auto serialized = table.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "SinLookupTable");
    EXPECT_EQ(
        serialized["components"].dump(),
        "[{\"name\":\"data\",\"type\":\"LookupTable\",\"parameters\":[],\"components\":[]}]"
    );
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests SinLookupTableTest component interpolating a couple of points
TEST_F(SinLookupTableTest, SinLookupTableInterpolation)
{
    std::string    name = "table";
    SinLookupTable table(name, nullptr, 10000);

    EXPECT_NEAR(table.interpolate(0.0), sin(0.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI / 2.0), sin(M_PI / 2.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI), sin(M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(4.0 / 3.0 * M_PI), sin(4.0 / 3.0 * M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(2.0 * M_PI - 0.01), sin(2.0 * M_PI - 0.01), 1e-6);
}

//! Tests SinLookupTable component interpolating a couple of points that fall out of 0 - 2pi range
TEST_F(SinLookupTableTest, SinLookupTableInterpolationOutOfBounds)
{
    std::string    name = "table";
    SinLookupTable table(name, nullptr, 10000);

    const double epsilon = 1e-2;

    EXPECT_NEAR(table.interpolate(-M_PI), sin(-M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(-M_PI / 2.0), sin(-M_PI / 2.0), 1e-3);
    EXPECT_NEAR(table.interpolate(-M_PI), sin(-M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(-4.0 / 3.0 * M_PI), sin(-4.0 / 3.0 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(-2.0 * M_PI + epsilon), sin(-2.0 * M_PI + epsilon), 1e-3);

    EXPECT_NEAR(table.interpolate(5.5 * M_PI), sin(5.5 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(15.3 * M_PI / 2.0), sin(15.3 * M_PI / 2.0), 1e-3);
    EXPECT_NEAR(table.interpolate(7.939 * M_PI), sin(7.939 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(11.5 * 4.0 / 3.0 * M_PI), sin(11.5 * 4.0 / 3.0 * M_PI), 1e-3);
    EXPECT_NEAR(table.interpolate(123 * 2.0 * M_PI), sin(123 * 2.0 * M_PI), 1e-3);
}

//! Tests SinLookupTable consistency of interface
TEST_F(SinLookupTableTest, SinLookupTableConsistency)
{
    std::string    name = "table";
    SinLookupTable table(name, nullptr, 10000);

    EXPECT_NEAR(table.interpolate(0.0), table(0.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI / 2.0), table(M_PI / 2.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI), table(M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(4.0 / 3.0 * M_PI), table(4.0 / 3.0 * M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(2.0 * M_PI - 0.01), table(2.0 * M_PI - 0.01), 1e-6);
}
