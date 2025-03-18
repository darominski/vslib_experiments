//! @file
//! @brief File with unit tests for Dq0ToAlphaBetaTransform component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "csv.hpp"
#include "dq0ToAlphaBetaTransform.hpp"
#include "rootComponent.hpp"

using namespace vslib;
using namespace csv;

class Dq0ToAlphaBetaTransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of Dq0ToAlphaBetaTransform component
TEST_F(Dq0ToAlphaBetaTransformTest, Construction)
{
    RootComponent           root;
    std::string_view        name = "dq0_to_alphaBeta_1";
    Dq0ToAlphaBetaTransform transform(name, root);
    ASSERT_EQ(transform.getName(), name);

    auto serialized = transform.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "Dq0ToAlphaBetaTransform");
    EXPECT_EQ(serialized["components"].size(), 2);
    EXPECT_EQ(
        serialized["components"].dump(),
        "[{\"name\":\"sin\",\"type\":\"SinLookupTable\",\"parameters\":[],\"components\":[{\"name\":\"data\",\"type\":"
        "\"LookupTable\",\"parameters\":[],\"components\":[]}]},{\"name\":\"cos\",\"type\":\"CosLookupTable\","
        "\"parameters\":[],\"components\":[{\"name\":\"data\",\"type\":\"LookupTable\",\"parameters\":[],"
        "\"components\":[]}]}]"
    );
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

TEST_F(Dq0ToAlphaBetaTransformTest, BasicTest)
{
    RootComponent           root;
    std::string_view        name = "dq0_to_alphaBeta_2";
    Dq0ToAlphaBetaTransform transform(name, root, 10'000);

    const double d           = 1.0;
    const double q           = -0.5;
    const double i_zero      = 0.0;
    const double theta       = M_PI / 6;   // 30 degrees in radians
    const bool   a_alignment = true;
    auto [alpha, beta, zero] = transform.transform(d, q, i_zero, theta, a_alignment);

    // Expected values calculation
    const double expected_alpha = d * cos(theta) - q * sin(theta);
    const double expected_beta  = d * sin(theta) + q * cos(theta);
    const double expected_zero  = i_zero;

    EXPECT_NEAR(alpha, expected_alpha, 1e-6);
    EXPECT_NEAR(beta, expected_beta, 1e-6);
    EXPECT_NEAR(zero, expected_zero, 1e-6);
}

TEST_F(Dq0ToAlphaBetaTransformTest, ZeroAngleTest)
{
    RootComponent           root;
    std::string_view        name = "dq0_to_alphaBeta_3";
    Dq0ToAlphaBetaTransform transform(name, root);

    const double d           = 1.0;
    const double q           = -0.5;
    const double i_zero      = -0.5;
    const double theta       = 0.0;
    const bool   a_alignment = true;
    auto [alpha, beta, zero] = transform.transform(d, q, i_zero, theta, a_alignment);

    // Expected values calculation
    const double expected_alpha = d * cos(theta) - q * sin(theta);
    const double expected_beta  = d * sin(theta) + q * cos(theta);
    const double expected_zero  = i_zero;

    EXPECT_NEAR(d, expected_alpha, 1e-6);
    EXPECT_NEAR(beta, expected_beta, 1e-6);
    EXPECT_NEAR(zero, expected_zero, 1e-6);
}

TEST_F(Dq0ToAlphaBetaTransformTest, ZeroAngleTestNotAligned)
{
    RootComponent           root;
    std::string_view        name = "dq0_to_alphaBeta_3";
    Dq0ToAlphaBetaTransform transform(name, root);

    const double d           = 1.0;
    const double q           = -0.5;
    const double i_zero      = -0.5;
    const double theta       = 0.0;
    const bool   a_alignment = false;
    auto [alpha, beta, zero] = transform.transform(d, q, i_zero, theta, a_alignment);

    // Expected values calculation
    const double expected_alpha = d * sin(theta) + q * cos(theta);
    const double expected_beta  = -d * cos(theta) + q * sin(theta);
    const double expected_zero  = i_zero;

    EXPECT_NEAR(alpha, expected_alpha, 1e-6);
    EXPECT_NEAR(beta, expected_beta, 1e-6);
    EXPECT_NEAR(zero, expected_zero, 1e-6);
}

//! Tests the transformation against the simulink output
TEST_F(Dq0ToAlphaBetaTransformTest, SimulinkConsistencyAaxisAlignment)
{
    RootComponent           root;
    std::string_view        name = "dq0_to_alphaBeta_5";
    Dq0ToAlphaBetaTransform transform(name, root);

    // the input files are randomly generated numbers
    std::filesystem::path abz_path   = "components/inputs/alpha-beta-zero_sin_120degrees.csv";
    std::filesystem::path theta_path = "components/inputs/park_theta_0_20.csv";
    std::filesystem::path dq0_path   = "components/inputs/alpha-beta-zero_to_dq0_sin_120degrees_theta_0_20.csv";

    CSVReader dq0_file(dq0_path.c_str());
    CSVReader theta_file(theta_path.c_str());
    CSVReader abz_file(abz_path.c_str());

    auto dq0_line   = dq0_file.begin();
    auto theta_line = theta_file.begin();
    auto abz_line   = abz_file.begin();

    while (dq0_line != dq0_file.end() && theta_line != theta_file.end() && abz_line != abz_file.end())
    {
        const auto d = (*dq0_line)[0].get<double>();
        const auto q = (*dq0_line)[1].get<double>();
        const auto z = (*dq0_line)[2].get<double>();

        const auto theta = (*theta_line)[1].get<double>();

        const auto matlab_alpha = (*abz_line)[0].get<double>();
        const auto matlab_beta  = (*abz_line)[1].get<double>();
        const auto matlab_zero  = (*abz_line)[2].get<double>();

        const auto [alpha, beta, zero] = transform.transform(d, q, z, theta);
        const auto relative_alpha      = (matlab_alpha - alpha);
        const auto relative_beta       = (matlab_beta - beta);
        const auto relative_zero       = (matlab_zero - zero);

        ASSERT_NEAR(relative_alpha, 0.0, 1e-6);   // at least 1e-6 relative precision
        ASSERT_NEAR(relative_beta, 0.0, 1e-6);    // at least 1e-6 relative precision
        ASSERT_NEAR(relative_zero, 0.0, 1e-6);    // at least 1e-6 relative precision

        ++dq0_line;
        ++theta_line;
        ++abz_line;
    }
}

//! Tests the transformation validation against simulink output
TEST_F(Dq0ToAlphaBetaTransformTest, SimulinkConsistencyAaxisNotAligned)
{
    RootComponent           root;
    std::string_view        name = "dq0_to_alphaBeta_6";
    Dq0ToAlphaBetaTransform transform(name, root);

    // the input files are randomly generated numbers
    std::filesystem::path abz_path   = "components/inputs/alpha-beta-zero_sin_120degrees.csv";
    std::filesystem::path theta_path = "components/inputs/park_theta_0_20.csv";
    std::filesystem::path dq0_path
        = "components/inputs/alpha-beta-zero_to_dq0_sin_120degrees_theta_0_20_a_notaligned.csv";

    CSVReader dq0_file(dq0_path.c_str());
    CSVReader theta_file(theta_path.c_str());
    CSVReader abz_file(abz_path.c_str());

    auto dq0_line   = dq0_file.begin();
    auto theta_line = theta_file.begin();
    auto abz_line   = abz_file.begin();

    while (dq0_line != dq0_file.end() && theta_line != theta_file.end() && abz_line != abz_file.end())
    {
        const auto d = (*dq0_line)[0].get<double>();
        const auto q = (*dq0_line)[1].get<double>();
        const auto z = (*dq0_line)[2].get<double>();

        const auto theta = (*theta_line)[1].get<double>();

        const auto matlab_alpha = (*abz_line)[0].get<double>();
        const auto matlab_beta  = (*abz_line)[1].get<double>();
        const auto matlab_zero  = (*abz_line)[2].get<double>();

        const auto [alpha, beta, zero] = transform.transform(d, q, z, theta, false);
        const auto relative_alpha      = (matlab_alpha - alpha);
        const auto relative_beta       = (matlab_beta - beta);
        const auto relative_zero       = (matlab_zero - zero);

        ASSERT_NEAR(relative_alpha, 0.0, 1e-6);   // at least 1e-6 relative precision
        ASSERT_NEAR(relative_beta, 0.0, 1e-6);    // at least 1e-6 relative precision
        ASSERT_NEAR(relative_zero, 0.0, 1e-6);    // at least 1e-6 relative precision

        ++dq0_line;
        ++theta_line;
        ++abz_line;
    }
}
