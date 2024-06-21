//! @file
//! @brief File with unit tests for ClarkeTransform component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "clarkeTransform.h"

using namespace vslib;

class ClarkeTransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of ClarkeTransform component
TEST_F(ClarkeTransformTest, Construction)
{
    std::string_view name = "clarke1";
    ClarkeTransform  clarke(name, nullptr);
    ASSERT_EQ(clarke.getName(), "clarke1");

    auto serialized = clarke.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "ClarkeTransform");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests basic case of ClarkeTransform for a balance system
TEST_F(ClarkeTransformTest, BasicTest)
{
    std::string_view name = "clarke2";
    ClarkeTransform  clarke(name, nullptr);

    double i_a               = 1.0;
    double i_b               = -0.5;
    double i_c               = -(i_a + i_b);
    auto [alpha, beta, zero] = clarke.transform(i_a, i_b, i_c);

    EXPECT_NEAR(alpha, i_a, 1e-6);
    EXPECT_NEAR(beta, (i_a + i_b * 2.0) / sqrt(3), 1e-6);
    EXPECT_NEAR(zero, 0.0, 1e-6);
}

//! Tests zero-sequence system (all phases are the same)
TEST(ClarkeTransformationTest, ZeroSequenceTest)
{
    std::string_view name = "clarke3";
    ClarkeTransform  clarke(name, nullptr);

    double i_a               = 1.0;
    double i_b               = 1.0;
    double i_c               = 1.0;
    auto [alpha, beta, zero] = clarke.transform(i_a, i_b, i_c);

    EXPECT_NEAR(alpha, 0.0, 1e-6);
    EXPECT_NEAR(beta, 0.0, 1e-6);
    EXPECT_NEAR(zero, 1.0, 1e-6);
}

//! Tests an unbalanced system
TEST(ClarkeTransformationTest, UnbalancedSystemTest)
{
    std::string_view name = "clarke4";
    ClarkeTransform  clarke(name, nullptr);

    double i_a               = 2.0;
    double i_b               = -1.0;
    double i_c               = -1.0;
    auto [alpha, beta, zero] = clarke.transform(i_a, i_b, i_c);

    EXPECT_NEAR(alpha, 2.0, 1e-6);
    EXPECT_NEAR(beta, 0.0, 1e-6);
    EXPECT_NEAR(zero, 0.0, 1e-6);
}

//! Tests interacting with transform method of ClarkeTransform component, validation against simulink
TEST_F(ClarkeTransformTest, SimulinkConsistency)
{
    std::string_view name = "clarke5";
    ClarkeTransform  clarke(name, nullptr);

    // the input files are randomly generated numbers
    std::filesystem::path abc_path    = "components/inputs/park_abc_sin_120degrees.csv";
    std::filesystem::path clarke_path = "components/inputs/clarke_alpha-beta_sin_120degrees.csv";

    std::ifstream abc_file(abc_path);
    std::ifstream clarke_file(clarke_path);

    ASSERT_TRUE(abc_file.is_open());
    ASSERT_TRUE(clarke_file.is_open());

    std::string abc_line;
    std::string clarke_line;

    while (getline(abc_file, abc_line) && getline(clarke_file, clarke_line))
    {
        // matlab inputs:
        std::stringstream ss(abc_line);
        std::string       timestamp, a_str, b_str, c_str;

        // Get the timestamp (we don't need it)
        std::getline(ss, timestamp, ',');

        // Get the a value
        std::getline(ss, a_str, ',');
        const auto a = std::stod(a_str);

        // Get the b value
        std::getline(ss, b_str, ',');
        const auto b = std::stod(b_str);

        // Get the c value
        std::getline(ss, c_str, ',');
        const auto c = std::stod(c_str);

        // matlab outputs
        std::string       alpha_str, beta_str;
        std::stringstream ss_clarke(clarke_line);
        std::getline(ss_clarke, alpha_str, ',');
        const auto matlab_alpha = std::stod(alpha_str);

        std::getline(ss_clarke, beta_str, ',');
        const auto matlab_beta = std::stod(beta_str);

        // validation
        const auto [alpha, beta, zero] = clarke.transform(a, b, c);

        EXPECT_NEAR(alpha, matlab_alpha, 1e-6);   // at least 1e-6 relative precision
        EXPECT_NEAR(beta, matlab_beta, 1e-6);     // at least 1e-6 relative precision
    }
    abc_file.close();
    clarke_file.close();
}