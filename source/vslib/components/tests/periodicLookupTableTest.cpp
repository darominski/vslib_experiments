//! @file
//! @brief File with unit tests of PeriodicLookupTable component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "periodicLookupTable.h"
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
    std::string                  name = "table";
    PeriodicLookupTable<int32_t> table(name, nullptr);
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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);
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
    std::string              name = "table";
    PeriodicLookupTable<int> table(name, nullptr);

    std::vector<std::pair<int, int>> values{{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    table.setData(std::move(values));

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
    std::string              name = "table";
    PeriodicLookupTable<int> table(name, nullptr);

    std::vector<std::pair<int, int>> values{{-3.0, 3}, {-2.0, 2}, {-1.0, 1}, {0.0, 0}};
    table.setData(std::move(values));

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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    table.setData(std::move(values));

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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    table.setData(std::move(values), true);

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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{0.0, 0.3}, {1.0, 1.3}, {2.0, 2.3}, {3.0, 3.3}};
    table.setData(std::move(values));

    EXPECT_EQ(table[0], 0.3);
    EXPECT_EQ(table[1], 1.3);
    EXPECT_EQ(table[2], 2.3);
    EXPECT_EQ(table[3], 3.3);
}

//! Tests PeriodicLookupTable component with a meaningful double table and then interpolating with trivial case of
//! hitting the provided points
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleNegativeAxis)
{
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    table.setData(std::move(values));

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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{-3.0, 3.3}, {-2.0, 2.3}, {-1.0, 1.3}, {0.0, 0.3}};
    table.setData(std::move(values));

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
    std::string                      name = "table";
    PeriodicLookupTable<double, int> table(name, nullptr);

    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    table.setData(std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point, with constant
//! binning
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableIntRepeatedInputConstantBinning)
{
    std::string                      name = "table";
    PeriodicLookupTable<double, int> table(name, nullptr);

    std::vector<std::pair<double, int>> values{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    table.setData(std::move(values), true);

    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
    EXPECT_EQ(table.interpolate(-2.5), static_cast<int>(0.5 * (3 + 2)));
}


//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedInput)
{
    std::string                         name = "table";
    PeriodicLookupTable<double, double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    table.setData(std::move(values));

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point, with contant
//! binning
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedInputConstantBinning)
{
    std::string                         name = "table";
    PeriodicLookupTable<double, double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    table.setData(std::move(values), true);

    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5), 0.5 * (3.3 + 2.2));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same point, with random
//! access
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedInputRandomAccess)
{
    std::string                         name = "table";
    PeriodicLookupTable<double, double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    table.setData(std::move(values));

    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
    EXPECT_EQ(table.interpolate(-2.5, true), 0.5 * (3.3 + 2.2));
}

//! Tests PeriodicLookupTable provides the same answer when repeatedly accessing the exact same section
TEST_F(PeriodicLookupTableTest, PeriodicLookupTableDoubleRepeatedSectionMonotonicallyIncreasing)
{
    std::string                         name = "table";
    PeriodicLookupTable<double, double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    table.setData(std::move(values));

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
    std::string                         name = "table";
    PeriodicLookupTable<double, double> table(name, nullptr);

    std::vector<std::pair<double, double>> values{{-3, 3.3}, {-2, 2.2}, {-1, 1.1}, {0, 0}};
    table.setData(std::move(values));

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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);
    table.setData(func());

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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);
    table.setData(func());

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
    std::string                      name = "table";
    PeriodicLookupTable<double, int> table(name, nullptr);

    std::vector<std::pair<double, int>> data{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    table.setData(std::move(data));

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
    std::string                 name = "table";
    PeriodicLookupTable<double> table(name, nullptr);

    std::vector<std::pair<double, double>> data{{-3, 3}, {-2, 2}, {-1, 1}, {0, 0}};
    table.setData(std::move(data));

    EXPECT_EQ(table.interpolate(-3.5), table.interpolate(-3.5, true));
    EXPECT_EQ(table.interpolate(-3), table.interpolate(-3, true));
    EXPECT_EQ(table.interpolate(-2.5), table.interpolate(-2.5, true));
    EXPECT_EQ(table.interpolate(-2), table.interpolate(-2, true));
    EXPECT_EQ(table.interpolate(-1.5), table.interpolate(-1.5, true));
    EXPECT_EQ(table.interpolate(-1), table.interpolate(-1, true));
    EXPECT_EQ(table.interpolate(-0.5), table.interpolate(-0.5, true));
    EXPECT_EQ(table.interpolate(0), table.interpolate(0, true));
}