//! @file
//! @brief File with unit tests for Dq0ToAbcTransform component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "csv.hpp"
#include "dq0ToAbcTransform.h"
#include "rootComponent.h"

using namespace vslib;
using namespace csv;

class Dq0ToAbcTransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of Dq0ToAbcTransform component
TEST_F(Dq0ToAbcTransformTest, Construction)
{
    RootComponent     root;
    std::string_view  name = "dq0_to_abc_1";
    Dq0ToAbcTransform dq0_to_abc(name, root);
    ASSERT_EQ(dq0_to_abc.getName(), name);

    auto serialized = dq0_to_abc.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "Dq0ToAbcTransform");
    EXPECT_EQ(serialized["components"].size(), 2);
    EXPECT_EQ(
        serialized["components"].dump(),
        "[{\"name\":\"dq0_to_alphabeta\",\"type\":\"Dq0ToAlphaBetaTransform\",\"parameters\":[],\"components\":[{"
        "\"name\":\"sin\",\"type\":\"SinLookupTable\",\"parameters\":[],\"components\":[{\"name\":\"data\",\"type\":"
        "\"LookupTable\",\"parameters\":[],\"components\":[]}]},{\"name\":\"cos\",\"type\":\"CosLookupTable\","
        "\"parameters\":[],\"components\":[{\"name\":\"data\",\"type\":\"LookupTable\",\"parameters\":[],"
        "\"components\":[]}]}]},{\"name\":\"alphabeta_to_abc\",\"type\":\"AlphaBetaToAbcTransform\",\"parameters\":[],"
        "\"components\":[]}]"
    );
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests a basic case of dq0 to abc transformation with arbitrary input
TEST_F(Dq0ToAbcTransformTest, BasicTest)
{
    RootComponent     root;
    std::string_view  name = "dq0_to_abc_2";
    Dq0ToAbcTransform dq0_to_abc(name, root, 10'000);   // 10k points needed for 1e-6 precision

    double d       = 1.0;
    double q       = -0.5;
    double zero    = -0.05;
    double theta   = M_PI / 6;   // 30 degrees in radians
    auto [a, b, c] = dq0_to_abc.transform(d, q, zero, theta);

    // Expected values calculation
    const double cos_theta                  = std::cos(theta);
    const double cos_theta_plus_two_thirds  = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_minus_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta                  = std::sin(theta);
    const double sin_theta_plus_two_thirds  = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_minus_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    double expected_a = d * sin_theta + q * cos_theta + zero;
    double expected_b = d * sin_theta_minus_two_thirds + q * cos_theta_minus_two_thirds + zero;
    double expected_c = d * sin_theta_plus_two_thirds + q * cos_theta_plus_two_thirds + zero;

    EXPECT_NEAR(a, expected_a, 1e-6);
    EXPECT_NEAR(b, expected_b, 1e-6);
    EXPECT_NEAR(c, expected_c, 1e-6);
}

//! Tests interacting with transform when the theta is equal to zero
TEST_F(Dq0ToAbcTransformTest, ZeroAngleTest)
{
    RootComponent     root;
    std::string_view  name = "dq0_to_abc_3";
    Dq0ToAbcTransform dq0_to_abc(name, root, 10'000);   // 10k points needed for 1e-6 precision

    double d       = 1.0;
    double q       = -0.5;
    double zero    = -0.05;
    double theta   = 0;   // 0 degrees
    auto [a, b, c] = dq0_to_abc.transform(d, q, zero, theta);

    // Expected values calculation
    const double cos_theta                  = std::cos(theta);
    const double cos_theta_plus_two_thirds  = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_minus_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta                  = std::sin(theta);
    const double sin_theta_plus_two_thirds  = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_minus_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    double expected_a = d * sin_theta + q * cos_theta + zero;
    double expected_b = d * sin_theta_minus_two_thirds + q * cos_theta_minus_two_thirds + zero;
    double expected_c = d * sin_theta_plus_two_thirds + q * cos_theta_plus_two_thirds + zero;

    EXPECT_NEAR(a, expected_a, 1e-6);
    EXPECT_NEAR(b, expected_b, 1e-6);
    EXPECT_NEAR(c, expected_c, 1e-6);
}

//! Tests interacting with transform when the offset is equal to 90 degrees
TEST_F(Dq0ToAbcTransformTest, ZeroAngle90degreesOffsetTest)
{
    RootComponent     root;
    std::string_view  name = "dq0_to_abc_4";
    Dq0ToAbcTransform dq0_to_abc(name, root, 10'000);

    double       d      = 1.0;
    double       q      = -0.5;
    double       zero   = -0.05;
    const double theta  = 0.0;
    const double offset = M_PI / 2.0;
    auto [a, b, c]      = dq0_to_abc.transform(d, q, zero, theta, offset);

    // Expected values calculation
    const double cos_theta                  = std::cos(theta + offset);
    const double cos_theta_plus_two_thirds  = std::cos(theta + 2.0 * M_PI / 3.0 + offset);
    const double cos_theta_minus_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0 + offset);

    const double sin_theta                  = std::sin(theta + offset);
    const double sin_theta_plus_two_thirds  = std::sin(theta + 2.0 * M_PI / 3.0 + offset);
    const double sin_theta_minus_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0 + offset);

    double expected_a = d * sin_theta + q * cos_theta + zero;
    double expected_b = d * sin_theta_minus_two_thirds + q * cos_theta_minus_two_thirds + zero;
    double expected_c = d * sin_theta_plus_two_thirds + q * cos_theta_plus_two_thirds + zero;

    EXPECT_NEAR(a, expected_a, 1e-6);
    EXPECT_NEAR(b, expected_b, 1e-6);
    EXPECT_NEAR(c, expected_c, 1e-6);
}

//! Tests interacting with transform when the theta is equal to 90 degrees
TEST_F(Dq0ToAbcTransformTest, NinetyDegreesTest)
{
    RootComponent     root;
    std::string_view  name = "dq0_to_abc_5";
    Dq0ToAbcTransform dq0_to_abc(name, root, 10'000);

    double d       = 1.0;
    double q       = -0.5;
    double zero    = -0.5;
    double theta   = M_PI / 2;   // 90 degrees in radians
    auto [a, b, c] = dq0_to_abc.transform(d, q, zero, theta);

    // Expected values calculation
    const double cos_theta                  = std::cos(theta);
    const double cos_theta_plus_two_thirds  = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_minus_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta                  = std::sin(theta);
    const double sin_theta_plus_two_thirds  = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_minus_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    double expected_a = d * sin_theta + q * cos_theta + zero;
    double expected_b = d * sin_theta_minus_two_thirds + q * cos_theta_minus_two_thirds + zero;
    double expected_c = d * sin_theta_plus_two_thirds + q * cos_theta_plus_two_thirds + zero;

    EXPECT_NEAR(a, expected_a, 1e-5);
    EXPECT_NEAR(b, expected_b, 1e-5);
    EXPECT_NEAR(c, expected_c, 1e-5);
}

//! Tests interacting with transform method of Dq0ToAbcTransform component, validation against simulink
TEST_F(Dq0ToAbcTransformTest, BasicSimulinkConsistency)
{
    RootComponent     root;
    std::string_view  name = "dq0_to_abc_6";
    Dq0ToAbcTransform dq0_to_abc(name, root, 10'000);

    // the input files are randomly generated numbers
    std::filesystem::path dq0_path   = "components/inputs/park_dq0_sin_120degrees_theta_0_20.csv";
    std::filesystem::path abc_path   = "components/inputs/park_abc_sin_120degrees.csv";
    std::filesystem::path theta_path = "components/inputs/park_theta_0_20.csv";

    CSVReader abc_file(abc_path.c_str());
    CSVReader dq0_file(dq0_path.c_str());
    CSVReader theta_file(theta_path.c_str());

    auto abc_line   = abc_file.begin();
    auto dq0_line   = dq0_file.begin();
    auto theta_line = theta_file.begin();

    while (abc_line != abc_file.end() && dq0_line != dq0_file.end() && theta_line != theta_file.end())
    {
        const auto matlab_a = (*abc_line)[1].get<double>();
        const auto matlab_b = (*abc_line)[2].get<double>();
        const auto matlab_c = (*abc_line)[3].get<double>();

        const auto theta = (*theta_line)[1].get<double>();

        const auto d = (*dq0_line)[0].get<double>();
        const auto q = (*dq0_line)[1].get<double>();
        const auto z = (*dq0_line)[2].get<double>();

        const auto [a, b, c]  = dq0_to_abc.transform(d, q, z, theta);
        const auto relative_a = (matlab_a - a);
        const auto relative_b = (matlab_b - b);
        const auto relative_c = (matlab_c - c);

        ASSERT_NEAR(relative_a, 0.0, 1e-6);   // at least 1e-6 relative precision
        ASSERT_NEAR(relative_b, 0.0, 1e-6);   // at least 1e-6 relative precision
        ASSERT_NEAR(relative_c, 0.0, 1e-6);   // at least 1e-6 relative precision

        ++abc_line;
        ++dq0_line;
        ++theta_line;
    }
}

//! Tests interacting with transform method of Dq0ToAbcTransform component, validation against simulink and SVC measured
//! data
TEST_F(Dq0ToAbcTransformTest, SVCTransform)
{
    RootComponent     root;
    std::string_view  name = "dq0_to_abc_7";
    Dq0ToAbcTransform dq0_to_abc(name, root);

    // the input files are randomly generated numbers
    std::filesystem::path abc_path   = "components/inputs/svc_18kV.csv";
    std::filesystem::path theta_path = "components/inputs/theta_svc_18kV_pll.csv";
    std::filesystem::path dq0_path   = "components/inputs/park_dq0_svc_18kV_pll.csv";

    CSVReader abc_file(abc_path.c_str());
    CSVReader dq0_file(dq0_path.c_str());
    CSVReader theta_file(theta_path.c_str());

    auto abc_line   = abc_file.begin();
    auto dq0_line   = dq0_file.begin();
    auto theta_line = theta_file.begin();

    while (abc_line != abc_file.end() && dq0_line != dq0_file.end() && theta_line != theta_file.end())
    {
        const auto matlab_a = (*abc_line)[1].get<double>();
        const auto matlab_b = (*abc_line)[2].get<double>();
        const auto matlab_c = (*abc_line)[3].get<double>();

        const auto theta = (*theta_line)[1].get<double>();

        const auto d = (*dq0_line)[0].get<double>();
        const auto q = (*dq0_line)[1].get<double>();
        const auto z = (*dq0_line)[2].get<double>();

        const auto [a, b, c]  = dq0_to_abc.transform(d, q, z, theta);
        const auto relative_a = (matlab_a - a);
        const auto relative_b = (matlab_b - b);
        const auto relative_c = (matlab_c - c);

        ASSERT_NEAR(relative_a, 0.0, 1e-6);   // at least 1e-6 relative precision
        ASSERT_NEAR(relative_b, 0.0, 1e-6);   // at least 1e-6 relative precision
        ASSERT_NEAR(relative_c, 0.0, 1e-6);   // at least 1e-6 relative precision

        ++abc_line;
        ++dq0_line;
        ++theta_line;
    }
}