//! @file
//! @brief File with unit tests of LookupTable component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "lookupTable.hpp"
#include "rootComponent.hpp"
#include "staticJson.hpp"

using namespace vslib;

class LookupTableTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};


//! Tests default construction of integral type LookupTable component
TEST_F(LookupTableTest, LookupTableIntDefault)
{
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    LookupTable<int32_t>             table(name, root, std::move(values));
    EXPECT_EQ(table.getName(), name);

    auto serialized = table.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LookupTable");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests default construction of double type LookupTable component
TEST_F(LookupTableTest, LookupTableDoubleDefault)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    LookupTable<double>                    table(name, root, std::move(values));
    EXPECT_EQ(table.getName(), name);

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
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    LookupTable<int>                 table(name, root, std::move(values));

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
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, root, std::move(values));

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
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    LookupTable<double>                    table(name, root, std::move(values));

    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
}

//! Tests LookupTable component with a assuming that the x-axis is constant-binned
TEST_F(LookupTableTest, LookupTableDoubleConstantBinning)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    LookupTable<double>                    table(name, root, std::move(values), true);

    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
}

//! Tests LookupTable's random access operator overload
TEST_F(LookupTableTest, LookupTableDoubleAccessOperatorOverload)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    LookupTable<double>                    table(name, root, std::move(values));

    EXPECT_EQ(table[0], 0.3);
    EXPECT_EQ(table[1], 1.3);
    EXPECT_EQ(table[2], 2.3);
    EXPECT_EQ(table[3], 3.3);
}

//! Tests LookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(LookupTableTest, LookupTableDoubleNegativeAxis)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    LookupTable<double>                    table(name, root, std::move(values));

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
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 3.3}, {1.0, 2.3}, {2.0, 1.3}, {3.0, 0.3}, {4.0, -0.3}};
    LookupTable<double>                    table(name, root, std::move(values));

    EXPECT_NEAR(table.interpolate(0.5), 0.5 * (3.3 + 2.3), 1e-15);
    EXPECT_NEAR(table.interpolate(1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(2.5), 0.5 * (1.3 + 0.3), 1e-15);

    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(2.5), 0.5 * (1.3 + 0.3), 1e-15);
    EXPECT_NEAR(table.interpolate(1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(0.5), 0.5 * (3.3 + 2.3), 1e-15);
}

//! Tests LookupTable component with a meaningful double table and then interpolating with a more realistic case of
//! interpolation input being somewhere between the data points
TEST_F(LookupTableTest, LookupTableDoubleInterpolateBetweenPointsNegativeAxis)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    LookupTable<double>                    table(name, root, std::move(values));

    EXPECT_NEAR(table.interpolate(-2.5), 0.5 * (3.3 + 2.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-0.5), 0.5 * (1.3 + 0.3), 1e-15);

    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(-0.5), 0.5 * (1.3 + 0.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-2.5), 0.5 * (3.3 + 2.3), 1e-15);
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point
TEST_F(LookupTableTest, LookupTableIntRepeatedInput)
{
    RootComponent                       root;
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double, int>            table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point, with constant binning
TEST_F(LookupTableTest, LookupTableIntRepeatedInputConstantBinning)
{
    RootComponent                       root;
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double, int>            table(name, root, std::move(values), true);

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point
TEST_F(LookupTableTest, LookupTableDoubleRepeatedInput)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point, with contant binning
TEST_F(LookupTableTest, LookupTableDoubleRepeatedInputConstantBinning)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, root, std::move(values), true);

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point, with random access
TEST_F(LookupTableTest, LookupTableDoubleRepeatedInputRandomAccess)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same section
TEST_F(LookupTableTest, LookupTableDoubleRepeatedSectionMonotonicallyIncreasing)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, root, std::move(values));

    const double interpolation_factor = (2.2 - 3.3) / (-2 + 3);
    for (int index = 1; index <= 10; index++)
    {
        double input = -3 + index / 10.0;
        EXPECT_NEAR(table.interpolate(input), 2.2 + (input + 2) * interpolation_factor, 1e-15);
    }
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same section
TEST_F(LookupTableTest, LookupTableDoubleRepeatedSectionMonotonicallyDecreasing)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, root, std::move(values));

    const double interpolation_factor = (2.2 - 3.3) / (-2 + 3);
    for (int index = 1; index <= 10; index++)
    {
        double input = -2 - index / 10.0;
        EXPECT_NEAR(table.interpolate(input), 2.2 + (input + 2) * interpolation_factor, 1e-15);
    }
}

//! Tests LookupTable provides the expected saturation behaviour the input is below the provided data limits
TEST_F(LookupTableTest, LookupTableIntInterpolateBelowLimitsConsistency)
{
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-4), 3);
    EXPECT_EQ(table.interpolate(-100), 3);
    EXPECT_EQ(table.interpolate(-15), table.interpolate(-1000));
}

//! Tests LookupTable provides the expected saturation behaviour the input is above the provided data limits
TEST_F(LookupTableTest, LookupTableIntInterpolateAboveLimits)
{
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(4), 0);
}

//! Tests LookupTable provides the expected output regardless whether the switch for random access is true or not
TEST_F(LookupTableTest, LookupTableIntRandomAccessConsistency)
{
    RootComponent                       root;
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double, int>            table(name, root, std::move(values));

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
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double>                    table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-3.5), table.interpolate(-3.5, true));
    EXPECT_EQ(table.interpolate(-3), table.interpolate(-3, true));
    EXPECT_EQ(table.interpolate(-2.5), table.interpolate(-2.5, true));
    EXPECT_EQ(table.interpolate(-2), table.interpolate(-2, true));
    EXPECT_EQ(table.interpolate(-1.5), table.interpolate(-1.5, true));
    EXPECT_EQ(table.interpolate(-1), table.interpolate(-1, true));
    EXPECT_EQ(table.interpolate(-0.5), table.interpolate(-0.5, true));
    EXPECT_EQ(table.interpolate(0), table.interpolate(0, true));
}

//! Tests LookupTable provides the expected output regardless whether the index-search or linear search is performed
TEST_F(LookupTableTest, LookupTableIntIndexSearchConsistency)
{
    RootComponent                       root;
    std::string                         name = "table";
    std::vector<std::pair<double, int>> data_index{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}, {1, 1}, {2, 2}};
    std::vector<std::pair<double, int>> data_linear(data_index);
    LookupTable<double, int>            table_linear(name, root, std::move(data_index), true);
    LookupTable<double, int>            table_index(name, root, std::move(data_linear), true);

    EXPECT_EQ(table_linear.interpolate(-3.5), table_index.interpolate(-3.5));
    EXPECT_EQ(table_linear.interpolate(-3), table_index.interpolate(-3));
    EXPECT_EQ(table_linear.interpolate(-2.5), table_index.interpolate(-2.5));
    EXPECT_EQ(table_linear.interpolate(-2), table_index.interpolate(-2));
    EXPECT_EQ(table_linear.interpolate(-1.5), table_index.interpolate(-1.5));
    EXPECT_EQ(table_linear.interpolate(-1), table_index.interpolate(-1));
    EXPECT_EQ(table_linear.interpolate(-0.5), table_index.interpolate(-0.5));
    EXPECT_EQ(table_linear.interpolate(0), table_index.interpolate(0));
}

//! Tests LookupTable provides the expected output regardless whether the index-search or linear search is performed
TEST_F(LookupTableTest, LookupTableDoubleIndexSearchConsistency)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> data_index{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}, {1, 1}, {2, 2}};
    std::vector<std::pair<double, double>> data_linear(data_index);
    LookupTable<double, double>            table_linear(name, root, std::move(data_index), true);
    LookupTable<double, double>            table_index(name, root, std::move(data_linear), true);

    EXPECT_EQ(table_linear.interpolate(-3.5), table_index.interpolate(-3.5));
    EXPECT_EQ(table_linear.interpolate(-3), table_index.interpolate(-3));
    EXPECT_EQ(table_linear.interpolate(-2.5), table_index.interpolate(-2.5));
    EXPECT_EQ(table_linear.interpolate(-2), table_index.interpolate(-2));
    EXPECT_EQ(table_linear.interpolate(-1.5), table_index.interpolate(-1.5));
    EXPECT_EQ(table_linear.interpolate(-1), table_index.interpolate(-1));
    EXPECT_EQ(table_linear.interpolate(-0.5), table_index.interpolate(-0.5));
    EXPECT_EQ(table_linear.interpolate(0), table_index.interpolate(0));
}
