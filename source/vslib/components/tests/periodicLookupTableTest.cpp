//! @file
//! @brief File with unit tests of PeriodicLookupTable component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "periodicLookupTable.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;

class PeriodicLookupTableTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

std::vector<std::pair<double, double>> func()
{
    constexpr size_t                       length = 1000;
    std::vector<std::pair<double, double>> func(length);
    for (size_t index = 0; index < length; index++)
    {
        func[index] = std::make_pair(
            index * 2 * M_PI / static_cast<double>(length - 1), sin(index * 2 * M_PI / static_cast<double>(length))
        );
    }
    return func;
}

//! Tests default construction of integral type PeriodicLookupTable component
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntDefault)
{
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    PeriodicLookupTable<int32_t>     table(name, root, std::move(values));
    EXPECT_EQ(table.getName(), name);

    auto serialized = table.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LookupTable");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests default construction of double type PeriodicLookupTable component
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleDefault)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    PeriodicLookupTable<double>            table(name, root, std::move(values));
    EXPECT_EQ(table.getName(), name);

    auto serialized = table.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LookupTable");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests PeriodicLookupTable component with a meaningful integer table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntInterpolateProvidedData)
{
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    PeriodicLookupTable<int>         table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(0), 0);
    EXPECT_EQ(table.interpolate(1), 1);
    EXPECT_EQ(table.interpolate(2), 2);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_EQ(table.interpolate(2), 2);
    EXPECT_EQ(table.interpolate(1), 1);
    EXPECT_EQ(table.interpolate(0), 0);
}

//! Tests PeriodicLookupTable component with a meaningful integer table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntInterpolateProvidedDataNegativeAxis)
{
    RootComponent                    root;
    std::string                      name = "table";
    std::vector<std::pair<int, int>> values{{-3.0, 3}, {-2.0, 2}, {-1.0, 1}, {0.0, 0}};
    PeriodicLookupTable<int>         table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(0), 0);
    EXPECT_EQ(table.interpolate(-1), 1);
    EXPECT_EQ(table.interpolate(-2), 2);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_EQ(table.interpolate(-2), 2);
    EXPECT_EQ(table.interpolate(-1), 1);
    EXPECT_EQ(table.interpolate(0), 0);
}

//! Tests PeriodicLookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleProvidedData)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    PeriodicLookupTable<double>            table(name, root, std::move(values));

    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
}

//! Tests PeriodicLookupTable component with a assuming that the x-axis is constant-binned
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleConstantBinning)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    PeriodicLookupTable<double>            table(name, root, std::move(values), true);

    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(0.0), 0.3, 1e-15);
}

//! Tests PeriodicLookupTable's random access operator overload
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleAccessOperatorOverload)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    PeriodicLookupTable<double>            table(name, root, std::move(values));

    EXPECT_EQ(table[0], 0.3);
    EXPECT_EQ(table[1], 1.3);
    EXPECT_EQ(table[2], 2.3);
    EXPECT_EQ(table[3], 3.3);
}

//! Tests PeriodicLookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleNegativeAxis)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    PeriodicLookupTable<double>            table(name, root, std::move(values));

    EXPECT_NEAR(table.interpolate(-3.0), 3.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-1.0), 1.3, 1e-15);

    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(-1.0), 1.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-2.0), 2.3, 1e-15);
    EXPECT_NEAR(table.interpolate(-3.0), 3.3, 1e-15);
}

//! Tests PeriodicLookupTable component with a meaningful double table and then interpolating with a more realistic case
//! of interpolation input being somewhere between the data points
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleInterpolateBetweenPoints)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    PeriodicLookupTable<double>            table(name, root, std::move(values));

    EXPECT_NEAR(table.interpolate(-2.5), 0.5 * (3.3 + 2.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-0.5), 0.5 * (1.3 + 0.3), 1e-15);

    // and check that nothing goes wrong if we do the same in reverse order:
    EXPECT_NEAR(table.interpolate(-0.5), 0.5 * (1.3 + 0.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-1.5), 0.5 * (2.3 + 1.3), 1e-15);
    EXPECT_NEAR(table.interpolate(-2.5), 0.5 * (3.3 + 2.3), 1e-15);
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntRepeatedInput)
{
    RootComponent                       root;
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    PeriodicLookupTable<double, int>    table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point, with constant
//! binning
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntRepeatedInputConstantBinning)
{
    RootComponent                       root;
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    PeriodicLookupTable<double, int>    table(name, root, std::move(values), true);

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}


//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedInput)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    PeriodicLookupTable<double, double>    table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point, with contant
//! binning
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedInputConstantBinning)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    PeriodicLookupTable<double, double>    table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point, with random
//! access
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedInputRandomAccess)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    PeriodicLookupTable<double, double>    table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same section
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedSectionMonotonicallyIncreasing)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    PeriodicLookupTable<double, double>    table(name, root, std::move(values));

    const double interpolation_factor = (2.2 - 3.3) / (-2 + 3);
    for (int index = 1; index <= 10; index++)
    {
        double input = -3 + index / 10.0;
        EXPECT_NEAR(table.interpolate(input), 2.2 + (input + 2) * interpolation_factor, 1e-15);
    }
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same section
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedSectionMonotonicallyDecreasing)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    PeriodicLookupTable<double, double>    table(name, root, std::move(values));

    const double interpolation_factor = (2.2 - 3.3) / (-2 + 3);
    for (int index = 1; index <= 10; index++)
    {
        double input = -2 - index / 10.0;
        EXPECT_NEAR(table.interpolate(input), 2.2 + (input + 2) * interpolation_factor, 1e-15);
    }
}

//! Tests PeriodicLookupTable provides the expected modulo behaviour the input is below the provided data limits
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleInterpolateBelowLimitsConsistency)
{
    RootComponent               root;
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, root, func());

    EXPECT_NEAR(table.interpolate(-2 * M_PI), table.interpolate(0), 1e-6);
    EXPECT_NEAR(table.interpolate(-3 * M_PI), table.interpolate(M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(-M_PI * 1.0 / 4.0), table.interpolate(M_PI * (2.0 - 1.0 / 4.0)), 1e-6);
    EXPECT_NEAR(table.interpolate(-M_PI * 1.0 / 2.0), table.interpolate(M_PI * (2 - 1.0 / 2.0)), 1e-6);
    EXPECT_NEAR(table.interpolate(-M_PI * 3.0 / 4.0), table.interpolate(M_PI * (2 - 3.0 / 4.0)), 1e-6);
    EXPECT_NEAR(table.interpolate(-M_PI * 6.0 / 4.0), table.interpolate(M_PI * (2 - 3.0 / 2.0)), 1e-6);
}

//! Tests PeriodicLookupTable provides the expected modulo behaviour the input is above the provided data limits
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntInterpolateAboveLimits)
{
    RootComponent               root;
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, root, func());

    EXPECT_NEAR(table.interpolate(3 * M_PI), table.interpolate(M_PI), 1e-6);
    EXPECT_NEAR(table.interpolate(4 * M_PI), table.interpolate(0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI * (2 + 1.0 / 4.0)), table.interpolate(M_PI * 1.0 / 4.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI * (2 + 1.0 / 2.0)), table.interpolate(M_PI * 1.0 / 2.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI * (2 + 3.0 / 4.0)), table.interpolate(M_PI * 3.0 / 4.0), 1e-6);
    EXPECT_NEAR(table.interpolate(M_PI * (2 + 3.0 / 2.0)), table.interpolate(M_PI * 3.0 / 2.0), 1e-6);
}

//! Tests PeriodicLookupTable provides the expected output regardless whether the switch for random access is true or
//! not
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntRandomAccessConsistency)
{
    RootComponent                       root;
    std::string                         name = "table";
    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    PeriodicLookupTable<double, int>    table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-3.5), table.interpolate(-3.5, true));
    EXPECT_EQ(table.interpolate(-3), table.interpolate(-3, true));
    EXPECT_EQ(table.interpolate(-2.5), table.interpolate(-2.5, true));
    EXPECT_EQ(table.interpolate(-2), table.interpolate(-2, true));
    EXPECT_EQ(table.interpolate(-1.5), table.interpolate(-1.5, true));
    EXPECT_EQ(table.interpolate(-1), table.interpolate(-1, true));
    EXPECT_EQ(table.interpolate(-0.5), table.interpolate(-0.5, true));
    EXPECT_EQ(table.interpolate(0), table.interpolate(0, true));
}

//! Tests PeriodicLookupTable provides the expected output regardless whether the switch for random access is true or
//! not
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRandomAccessConsistency)
{
    RootComponent                          root;
    std::string                            name = "table";
    std::vector<std::pair<double, double>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    PeriodicLookupTable<double>            table(name, root, std::move(values));

    EXPECT_EQ(table.interpolate(-3.5), table.interpolate(-3.5, true));
    EXPECT_EQ(table.interpolate(-3), table.interpolate(-3, true));
    EXPECT_EQ(table.interpolate(-2.5), table.interpolate(-2.5, true));
    EXPECT_EQ(table.interpolate(-2), table.interpolate(-2, true));
    EXPECT_EQ(table.interpolate(-1.5), table.interpolate(-1.5, true));
    EXPECT_EQ(table.interpolate(-1), table.interpolate(-1, true));
    EXPECT_EQ(table.interpolate(-0.5), table.interpolate(-0.5, true));
    EXPECT_EQ(table.interpolate(0), table.interpolate(0, true));
}