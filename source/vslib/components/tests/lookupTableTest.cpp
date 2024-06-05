//! @file
//! @brief File with unit tests of LookupTable component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

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
    }
};


//! Tests default construction of integral type LookupTable component
TEST_F(LookupTableTest, LookupTableIntDefault)
{
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    LookupTable<int32_t>             table(name, nullptr, std::move(values));
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
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));
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

//! Tests LookupTable component with a assuming that the x-axis is constant-binned
TEST_F(LookupTableTest, LookupTableDoubleConstantBinning)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values), true);

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
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

    EXPECT_EQ(table[0], 0.3);
    EXPECT_EQ(table[1], 1.3);
    EXPECT_EQ(table[2], 2.3);
    EXPECT_EQ(table[3], 3.3);
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
    std::vector<std::pair<double, double>> values{{0.0, 3.3}, {1.0, 2.3}, {2.0, 1.3}, {3.0, 0.3}, {4.0, -0.3}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

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

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point
TEST_F(LookupTableTest, LookupTableIntRepeatedInput)
{
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double, int>            table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point, with constant binning
TEST_F(LookupTableTest, LookupTableIntRepeatedInputConstantBinning)
{
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double, int>            table(name, nullptr, std::move(values), true);

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point
TEST_F(LookupTableTest, LookupTableDoubleRepeatedInput)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point, with contant binning
TEST_F(LookupTableTest, LookupTableDoubleRepeatedInputConstantBinning)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, nullptr, std::move(values), true);

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same point, with random access
TEST_F(LookupTableTest, LookupTableDoubleRepeatedInputRandomAccess)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
}

//! Tests LookupTable provides the same answer when repeatedly accessing the exact same section
TEST_F(LookupTableTest, LookupTableDoubleRepeatedSectionMonotonicallyIncreasing)
{
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, nullptr, std::move(values));

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
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    LookupTable<double, double>            table(name, nullptr, std::move(values));

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
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(-4), 3);
    EXPECT_EQ(table.interpolate(-100), 3);
    EXPECT_EQ(table.interpolate(-15), table.interpolate(-1000));
}

//! Tests LookupTable provides the expected saturation behaviour the input is above the provided data limits
TEST_F(LookupTableTest, LookupTableIntInterpolateAboveLimits)
{
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<int>                 table(name, nullptr, std::move(values));

    EXPECT_EQ(table.interpolate(4), 0);
}

//! Tests LookupTable provides the expected output regardless whether the switch for random access is true or not
TEST_F(LookupTableTest, LookupTableIntRandomAccessConsistency)
{
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double, int>            table(name, nullptr, std::move(values));

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
    std::vector<std::pair<double, double>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    LookupTable<double>                    table(name, nullptr, std::move(values));

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
    std::string                         name = "table";
    std::vector<std::pair<double, int>> data_index{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}, {1, 1}, {2, 2}};
    std::vector<std::pair<double, int>> data_linear(data_index);
    LookupTable<double, int>            table_linear(name, nullptr, std::move(data_index), true);
    LookupTable<double, int>            table_index(name, nullptr, std::move(data_linear), true);

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
    std::string                            name = "table";
    std::vector<std::pair<double, double>> data_index{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}, {1, 1}, {2, 2}};
    std::vector<std::pair<double, double>> data_linear(data_index);
    LookupTable<double, double>            table_linear(name, nullptr, std::move(data_index), true);
    LookupTable<double, double>            table_index(name, nullptr, std::move(data_linear), true);

    EXPECT_EQ(table_linear.interpolate(-3.5), table_index.interpolate(-3.5));
    EXPECT_EQ(table_linear.interpolate(-3), table_index.interpolate(-3));
    EXPECT_EQ(table_linear.interpolate(-2.5), table_index.interpolate(-2.5));
    EXPECT_EQ(table_linear.interpolate(-2), table_index.interpolate(-2));
    EXPECT_EQ(table_linear.interpolate(-1.5), table_index.interpolate(-1.5));
    EXPECT_EQ(table_linear.interpolate(-1), table_index.interpolate(-1));
    EXPECT_EQ(table_linear.interpolate(-0.5), table_index.interpolate(-0.5));
    EXPECT_EQ(table_linear.interpolate(0), table_index.interpolate(0));
}