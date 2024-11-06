//! @file
//! @brief File with unit tests for AlphaBetaToAbcTransform Component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "alphaBetaToAbcTransform.h"
#include "readCsv.h"
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
    std::filesystem::path abz_path = "components/inputs/clarke_alpha-beta_sin_120degrees.csv";
    std::filesystem::path abc_path = "components/inputs/park_abc_sin_120degrees.csv";

    fgc4::utils::test::ReadCSV<4> abc_file(abc_path);
    fgc4::utils::test::ReadCSV<3> abz_file(abz_path);

    while (!abz_file.eof() && !abc_file.eof())
    {
        const auto abc_line = abc_file.readLine();
        const auto abz_line = abz_file.readLine();

        if (abz_line && abc_line)
        {
            const auto [time1, matlab_a, matlab_b, matlab_c] = abc_line.value();
            const auto [alpha, beta, zero]                   = abz_line.value();

            // validation
            const auto [a, b, c] = inv_clarke.transform(alpha, beta, zero);

            ASSERT_NEAR(a, matlab_a, 1e-6);   // at least 1e-6 relative precision
            ASSERT_NEAR(b, matlab_b, 1e-6);   // at least 1e-6 relative precision
            ASSERT_NEAR(c, matlab_c, 1e-6);   // at least 1e-6 relative precision
        }
    }
}