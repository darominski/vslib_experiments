//! @file
//! @brief File with unit tests for AlphaBetaToAbcTransform Component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "alphaBetaToAbcTransform.h"
#include "rootComponent.h"

using namespace vslib;

class AlphaBetaToAbcTransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of AbcToAlphaBetaTransform component
TEST_F(AlphaBetaToAbcTransformTest, Construction)
{
    RootComponent           root;
    std::string_view        name = "inv_clarke_1";
    AlphaBetaToAbcTransform inv_clarke(name, root);
    ASSERT_EQ(inv_clarke.getName(), name);

    auto serialized = inv_clarke.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "AlphaBetaToAbcTransform");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests basic case of AbcToAlphaBetaTransform
TEST_F(AlphaBetaToAbcTransformTest, BasicTest)
{
    RootComponent           root;
    std::string_view        name = "inv_clarke_2";
    AlphaBetaToAbcTransform inv_clarke(name, root);

    double i_alpha = 1.0;
    double i_beta  = -0.5;
    double i_zero  = 0.1;
    auto [a, b, c] = inv_clarke.transform(i_alpha, i_beta, i_zero);

    EXPECT_NEAR(a, i_alpha + i_zero, 1e-6);
    EXPECT_NEAR(b, -0.5 * (i_alpha - sqrt(3) * i_beta) + i_zero, 1e-6);
    EXPECT_NEAR(c, -0.5 * (i_alpha + sqrt(3) * i_beta) + i_zero, 1e-6);
}

//! Tests zero-sequence system (all phases are the same)
TEST_F(AlphaBetaToAbcTransformTest, ZeroSequenceTest)
{
    RootComponent           root;
    std::string_view        name = "inv_clarke_3";
    AlphaBetaToAbcTransform inv_clarke(name, root);

    double i_alpha = 0.0;
    double i_beta  = 0.0;
    double i_zero  = 1.0;
    auto [a, b, c] = inv_clarke.transform(i_alpha, i_beta, i_zero);

    EXPECT_NEAR(a, 1.0, 1e-6);
    EXPECT_NEAR(b, 1.0, 1e-6);
    EXPECT_NEAR(c, 1.0, 1e-6);
}

//! Tests an unbalanced system
TEST_F(AlphaBetaToAbcTransformTest, UnbalancedSystemTest)
{
    RootComponent           root;
    std::string_view        name = "inv_clarke_4";
    AlphaBetaToAbcTransform inv_clarke(name, root);

    double i_alpha = 2.0;
    double i_beta  = 0.0;
    double i_zero  = 0.0;
    auto [a, b, c] = inv_clarke.transform(i_alpha, i_beta, i_zero);

    EXPECT_NEAR(a, 2.0, 1e-6);
    EXPECT_NEAR(b, -1.0, 1e-6);
    EXPECT_NEAR(c, -1.0, 1e-6);
}

//! Tests interacting with transform method of AlphaBetaToAbcTransformTest component, validation against simulink
TEST_F(AlphaBetaToAbcTransformTest, SimulinkConsistency)
{
    RootComponent           root;
    std::string_view        name = "inv_clarke_5";
    AlphaBetaToAbcTransform inv_clarke(name, root);

    // the input files are randomly generated numbers
    std::filesystem::path alphaBetaZero_path = "components/inputs/clarke_alpha-beta_sin_120degrees.csv";
    std::filesystem::path inv_clarke_path    = "components/inputs/park_abc_sin_120degrees.csv";

    std::ifstream alphaBetaZero_file(alphaBetaZero_path);
    std::ifstream inv_clarke_file(inv_clarke_path);

    ASSERT_TRUE(alphaBetaZero_file.is_open());
    ASSERT_TRUE(inv_clarke_file.is_open());

    std::string alphaBetaZero_line;
    std::string inv_clarke_line;

    while (getline(alphaBetaZero_file, alphaBetaZero_line) && getline(inv_clarke_file, inv_clarke_line))
    {
        // matlab inputs:
        std::stringstream ss(alphaBetaZero_line);
        std::string       alpha_str, beta_str, zero_str;

        // Get the a value
        std::getline(ss, alpha_str, ',');
        const auto alpha = std::stod(alpha_str);

        // Get the b value
        std::getline(ss, beta_str, ',');
        const auto beta = std::stod(beta_str);

        // Get the c value
        std::getline(ss, zero_str, ',');
        const auto zero = std::stod(zero_str);

        // matlab outputs
        std::string       timestamp, a_str, b_str, c_str;
        std::stringstream ss_clarke(inv_clarke_line);

        // Get the timestamp (we don't need it)
        std::getline(ss_clarke, timestamp, ',');

        std::getline(ss_clarke, a_str, ',');
        const auto matlab_a = std::stod(a_str);

        std::getline(ss_clarke, b_str, ',');
        const auto matlab_b = std::stod(b_str);

        std::getline(ss_clarke, c_str, ',');
        const auto matlab_c = std::stod(c_str);

        // validation
        const auto [a, b, c] = inv_clarke.transform(alpha, beta, zero);

        EXPECT_NEAR(a, matlab_a, 1e-6);   // at least 1e-6 relative precision
        EXPECT_NEAR(b, matlab_b, 1e-6);   // at least 1e-6 relative precision
        EXPECT_NEAR(c, matlab_c, 1e-6);   // at least 1e-6 relative precision
    }
    alphaBetaZero_file.close();
    inv_clarke_file.close();
}