//! @file
//! @brief File with unit tests of Limit component.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>

#include "limitRange.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;

class LimitRangeTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        ParameterRegistry& parameter_registry = ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    template<typename T>
    std::optional<fgc4::utils::Warning>
    set_limit_parameters(LimitRange<T>& limit, T min = T{}, T max = T{}, std::array<T, 2> dead_zone = {T{}, T{}})
    {
        StaticJson min_val = min;
        limit.min.setJsonValue(min_val);

        StaticJson max_val = max;
        limit.max.setJsonValue(max_val);

        StaticJson dead_zone_val = dead_zone;
        limit.dead_zone.setJsonValue(dead_zone_val);

        limit.flipBufferState();
        limit.synchroniseParameterBuffers();
        return limit.verifyParameters();
    }
};

// ************************************************************
// Basic construction tests for supported types

//! Tests default construction of integral type LimitRange component
TEST_F(LimitRangeTest, LimitIntegralDefault)
{
    RootComponent       root;
    std::string         name = "int_limit";
    LimitRange<int32_t> integral_limit(name, root);
    EXPECT_EQ(integral_limit.getName(), name);

    auto serialized = integral_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Int32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayInt32");
}

//! Tests default construction of unsigned integral type LimitRange component
TEST_F(LimitRangeTest, LimitUnsignedIntegralDefault)
{
    RootComponent        root;
    std::string          name = "uint_limit";
    LimitRange<uint32_t> uint_limit(name, root);

    auto serialized = uint_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "UInt32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayUInt32");
}

//! Tests default construction of float type LimitRange component
TEST_F(LimitRangeTest, LimitRangeFloatDefault)
{
    RootComponent     root;
    std::string       name = "float_limit";
    LimitRange<float> float_limit(name, root);

    auto serialized = float_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float32");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayFloat32");
}

//! Tests default construction of integral type LimitRange component
TEST_F(LimitRangeTest, LimitRangeDoubleDefault)
{
    RootComponent      root;
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, root);

    auto serialized = double_limit.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "LimitRange");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 3);
    EXPECT_EQ(serialized["parameters"][0]["name"], "lower_threshold");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][1]["name"], "upper_threshold");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][2]["name"], "dead_zone");
    EXPECT_EQ(serialized["parameters"][2]["type"], "ArrayFloat64");
}

// ************************************************************
// Basic min/max tests for supported types

//! Tests catching lower limit violation with int type
TEST_F(LimitRangeTest, LimitRangeIntMin)
{
    RootComponent   root;
    std::string     name = "int_limit";
    LimitRange<int> int_limit(name, root);

    const int min = -10;
    const int max = 10;
    set_limit_parameters<int>(int_limit, min, max);

    const int input = min - 2;

    const auto output = int_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, min);
}

//! Tests that min limit is exclusive, with int type
TEST_F(LimitRangeTest, LimitRangeIntMinExclusive)
{
    RootComponent   root;
    std::string     name = "int_limit";
    LimitRange<int> int_limit(name, root);

    const int min = -10;
    const int max = 10;
    set_limit_parameters<int>(int_limit, min, max);

    const int input = min;

    const auto output = int_limit.limit(input);
    ASSERT_EQ(input, output);
}

//! Tests that min limit is exclusive, with float type
TEST_F(LimitRangeTest, LimitRangeFloatMinExclusive)
{
    RootComponent     root;
    std::string       name = "float_limit";
    LimitRange<float> float_limit(name, root);

    const float min = -3.14159;
    const float max = 3.14159;
    set_limit_parameters<float>(float_limit, min, max);

    const float input = min;

    const auto output = float_limit.limit(input);
    ASSERT_EQ(input, output);
}

//! Tests catching lower limit violation with unsigned int type
TEST_F(LimitRangeTest, LimitRangeUIntMin)
{
    RootComponent        root;
    std::string          name = "uint_limit";
    LimitRange<uint64_t> uint_limit(name, root);

    const uint64_t min = 1;
    const uint64_t max = 1e3;
    set_limit_parameters<uint64_t>(uint_limit, min, max);

    const uint64_t input = 0;

    const auto output = uint_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, min);
}

//! Tests catching lower limit violation with double type
TEST_F(LimitRangeTest, LimitRangeDoubleMin)
{
    RootComponent      root;
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, root);

    const double min = 4;
    const double max = 1e3;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = min - 2;

    const auto output = double_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, min);
}

//! Tests catching upper limit violation with int type
TEST_F(LimitRangeTest, LimitRangeIntMax)
{
    RootComponent   root;
    std::string     name = "int_limit";
    LimitRange<int> int_limit(name, root);

    const int min = 4;
    const int max = 1e2;
    set_limit_parameters<int>(int_limit, min, max);

    const int input = max + 1;

    const auto output = int_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, max);
}

//! Tests catching upper limit violation with uint type
TEST_F(LimitRangeTest, LimitRangeUIntMax)
{
    RootComponent        root;
    std::string          name = "uint_limit";
    LimitRange<uint32_t> uint_limit(name, root);

    const uint32_t min = 4;
    const uint32_t max = 1e2;
    set_limit_parameters<uint32_t>(uint_limit, min, max);

    const uint32_t input = max + 1;

    const auto output = uint_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, max);
}

//! Tests catching upper limit violation with double type
TEST_F(LimitRangeTest, LimitRangeDoubleMax)
{
    RootComponent      root;
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, root);

    const double min = 4;
    const double max = 1e2;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = max + 1;

    const auto output = double_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, max);
}

// ************************************************************
// Basic dead-zone tests for supported types

//! Tests catching value in the dead zone
TEST_F(LimitRangeTest, LimitRangeIntDeadZone)
{
    RootComponent   root;
    std::string     name = "int_limit";
    LimitRange<int> limit(name, root);

    const int          min = -10;
    const int          max = 100;
    std::array<int, 2> dead_zone{0, 3};

    set_limit_parameters<int>(limit, min, max, dead_zone);

    const int input = dead_zone[0] + 1;

    const auto output = limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, dead_zone[0]);
}

//! Tests catching value in the dead zone
TEST_F(LimitRangeTest, LimitRangeUIntDeadZone)
{
    RootComponent        root;
    std::string          name = "int_limit";
    LimitRange<uint16_t> limit(name, root);

    const uint16_t          min = -10;
    const uint16_t          max = 100;
    std::array<uint16_t, 2> dead_zone{0, 4};

    set_limit_parameters<uint16_t>(limit, min, max, dead_zone);

    const uint16_t input = dead_zone[0] + 1;

    const auto output = limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, dead_zone[0]);
}

//! Tests catching value in the dead zone
TEST_F(LimitRangeTest, LimitRangeDoubleDeadZone)
{
    RootComponent      root;
    std::string        name = "double_limit";
    LimitRange<double> limit(name, root);

    const double          min = -10;
    const double          max = 100;
    std::array<double, 2> dead_zone{0.0, 3.0};

    set_limit_parameters<double>(limit, min, max, dead_zone);

    const double input = dead_zone[1] - 0.1;

    const auto output = limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, dead_zone[1]);
}

// ************************************************************
// Tests around numerical limits

//! Tests that min limit is exclusive, with int type near the numerical limit
TEST_F(LimitRangeTest, LimitRangeIntMinNumericalLimit)
{
    RootComponent   root;
    std::string     name = "int_limit";
    LimitRange<int> int_limit(name, root);

    const int min = std::numeric_limits<int>::lowest() + 1;
    const int max = 10;
    set_limit_parameters<int>(int_limit, min, max);

    const int input = std::numeric_limits<int>::lowest();

    const auto output = int_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, min);
}

//! Tests that min limit is exclusive, with float type near the numerical limit
TEST_F(LimitRangeTest, LimitRangeFloatMinNumericalLimit)
{
    RootComponent     root;
    std::string       name = "float_limit";
    LimitRange<float> float_limit(name, root);

    const float min = std::nextafterf(std::numeric_limits<float>::lowest(), 0.0);
    const float max = 3.14159;
    set_limit_parameters<float>(float_limit, min, max);

    const float input = min;

    const auto output = float_limit.limit(input);
    ASSERT_EQ(input, output);
}

//! Tests catching lower limit violation with double type near the numerical limit
TEST_F(LimitRangeTest, LimitRangeDoubleMinNumericalLimit)
{
    RootComponent      root;
    std::string        name = "float_limit";
    LimitRange<double> double_limit(name, root);

    const double min = std::nexttoward(std::numeric_limits<double>::lowest(), 0.0);
    const double max = 1e3;
    set_limit_parameters<double>(double_limit, min, max);

    const double input = std::numeric_limits<double>::lowest();

    const auto output = double_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, min);
}

//! Tests catching upper limit violation with int type close to the numerical limit
TEST_F(LimitRangeTest, LimitRangeIntMaxNumericalLimit)
{
    RootComponent       root;
    std::string         name = "int_limit";
    LimitRange<int64_t> int_limit(name, root);

    const int64_t min = 0;
    const int64_t max = std::numeric_limits<int64_t>::max() - 1;
    set_limit_parameters<int64_t>(int_limit, min, max);

    const int64_t input = std::numeric_limits<int64_t>::max();

    const auto output = int_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, max);
}

//! Tests catching upper limit violation with uint type close to the numerical limit
TEST_F(LimitRangeTest, LimitRangeUIntMaxNumericalLimit)
{
    RootComponent        root;
    std::string          name = "uint_limit";
    LimitRange<uint64_t> uint_limit(name, root);

    const uint64_t min = 0;
    const uint64_t max = std::numeric_limits<uint64_t>::max() - 1;
    set_limit_parameters<uint64_t>(uint_limit, min, max);

    const uint64_t input = std::numeric_limits<uint64_t>::max();

    const auto output = uint_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, max);
}

//! Tests catching upper limit violation with double type
TEST_F(LimitRangeTest, LimitRangeDoubleMaxNumericalLimit)
{
    RootComponent      root;
    std::string        name = "dbl_limit";
    LimitRange<double> double_limit(name, root);

    const double min = 4;
    const double max = std::nexttoward(std::numeric_limits<double>::max(), 0);
    set_limit_parameters<double>(double_limit, min, max);

    const double input = std::numeric_limits<double>::max();

    const auto output = double_limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, max);
}

// ************************************************************
// Tests with feeding infinity

//! Tests catching -inf being outside of limits with double type
TEST_F(LimitRangeTest, LimitRangeDoubleNegativeInf)
{
    RootComponent      root;
    std::string        name = "dbl_limit";
    LimitRange<double> limit(name, root);

    const double min = 4;
    const double max = 1e3;
    set_limit_parameters<double>(limit, min, max);

    const double input = -std::numeric_limits<double>::infinity();

    const auto output = limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, min);
}

//! Tests catching -inf being outside of limits with double type
TEST_F(LimitRangeTest, LimitRangeDoubleInf)
{
    RootComponent      root;
    std::string        name = "dbl_limit";
    LimitRange<double> limit(name, root);

    const double min = 4;
    const double max = 1e3;
    set_limit_parameters<double>(limit, min, max);

    const double input = std::numeric_limits<double>::infinity();

    const auto output = limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, max);
}

//! Tests catching -inf being outside of limits with double type
TEST_F(LimitRangeTest, LimitRangeDoubleNaN)
{
    RootComponent      root;
    std::string        name = "limit";
    LimitRange<double> limit(name, root);

    const double min = 4;
    const double max = 1e3;
    set_limit_parameters<double>(limit, min, max);

    const double input = std::numeric_limits<double>::quiet_NaN();

    const auto output = limit.limit(input);
    ASSERT_NE(input, output);
    ASSERT_EQ(output, std::numeric_limits<double>::min());
}
