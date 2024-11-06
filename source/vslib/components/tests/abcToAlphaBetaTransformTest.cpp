//! @file
//! @brief File with unit tests for AbcToAlphaBetaTransform component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "abcToAlphaBetaTransform.h"
#include "readCsv.h"
#include "rootComponent.h"

using namespace vslib;

class AbcToAlphaBetaTransformTest : public ::testing::Test
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
TEST_F(AbcToAlphaBetaTransformTest, Construction)
{
    RootComponent           root;
    std::string_view        name = "clarke1";
    AbcToAlphaBetaTransform clarke(name, root);
    ASSERT_EQ(clarke.getName(), "clarke1");

    auto serialized = clarke.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "AbcToAlphaBetaTransform");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests basic case of AbcToAlphaBetaTransform for a balanced system
TEST_F(AbcToAlphaBetaTransformTest, BasicTest)
{
    RootComponent           root;
    std::string_view        name = "clarke2";
    AbcToAlphaBetaTransform clarke(name, root);

    double i_a               = 1.0;
    double i_b               = -0.5;
    double i_c               = -(i_a + i_b);
    auto [alpha, beta, zero] = clarke.transform(i_a, i_b, i_c);

    EXPECT_NEAR(alpha, i_a, 1e-6);
    EXPECT_NEAR(beta, (i_a + i_b * 2.0) / sqrt(3), 1e-6);
    EXPECT_NEAR(zero, 0.0, 1e-6);
}

//! Tests zero-sequence system (all phases are the same)
TEST(AbcToAlphaBetaTransformationTest, ZeroSequenceTest)
{
    RootComponent           root;
    std::string_view        name = "clarke3";
    AbcToAlphaBetaTransform clarke(name, root);

    double i_a               = 1.0;
    double i_b               = 1.0;
    double i_c               = 1.0;
    auto [alpha, beta, zero] = clarke.transform(i_a, i_b, i_c);

    EXPECT_NEAR(alpha, 0.0, 1e-6);
    EXPECT_NEAR(beta, 0.0, 1e-6);
    EXPECT_NEAR(zero, 1.0, 1e-6);
}

//! Tests an unbalanced system
TEST(AbcToAlphaBetaTransformationTest, UnbalancedSystemTest)
{
    RootComponent           root;
    std::string_view        name = "clarke4";
    AbcToAlphaBetaTransform clarke(name, root);

    double i_a               = 2.0;
    double i_b               = -1.0;
    double i_c               = -1.0;
    auto [alpha, beta, zero] = clarke.transform(i_a, i_b, i_c);

    EXPECT_NEAR(alpha, 2.0, 1e-6);
    EXPECT_NEAR(beta, 0.0, 1e-6);
    EXPECT_NEAR(zero, 0.0, 1e-6);
}

//! Tests interacting with transform method of AbcToAlphaBetaTransform component, validation against simulink
TEST_F(AbcToAlphaBetaTransformTest, SimulinkConsistency)
{
    RootComponent           root;
    std::string_view        name = "clarke5";
    AbcToAlphaBetaTransform clarke(name, root);

    // the input files are randomly generated numbers
    std::filesystem::path abc_path = "components/inputs/park_abc_sin_120degrees.csv";
    std::filesystem::path abz_path = "components/inputs/clarke_alpha-beta_sin_120degrees.csv";

    fgc4::utils::test::ReadCSV<3> abz_file(abz_path);
    fgc4::utils::test::ReadCSV<4> abc_file(abc_path);

    while (!abc_file.eof() && !abz_file.eof())
    {
        const auto abz_line = abz_file.readLine();
        const auto abc_line = abc_file.readLine();

        if (abc_line && abz_line)
        {
            const auto [matlab_alpha, matlab_beta, matlab_zero] = abz_line.value();
            const auto [time1, a, b, c]                         = abc_line.value();

            // validation
            const auto [alpha, beta, zero] = clarke.transform(a, b, c);
            if (abs(matlab_alpha) > 1e-12)
            {
                ASSERT_NEAR((matlab_alpha - alpha) / matlab_alpha, 0.0, 1e-6);   // at least 1e-6 relative precision
            }
            else
            {
                ASSERT_NEAR(alpha, matlab_alpha, 1e-6);   // at least 1e-6 precision
            }

            if (abs(matlab_beta) > 1e-12)
            {
                ASSERT_NEAR((matlab_beta - beta) / matlab_beta, 0.0, 1e-6);   // at least 1e-6 relative precision
            }
            else
            {
                ASSERT_NEAR(beta, matlab_beta, 1e-6);   // at least 1e-6 precision
            }
        }
    }
}