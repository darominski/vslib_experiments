//! @file
//! @brief File with unit tests for AlphaBetaToDq0Transform component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "alphaBetaToDq0Transform.h"
#include "readCsv.h"
#include "rootComponent.h"

using namespace vslib;

class AlphaBetaToDq0TransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of AlphaBetaToDq0Transform component
TEST_F(AlphaBetaToDq0TransformTest, Construction)
{
    RootComponent           root;
    std::string_view        name = "AlphaBetaToDq0Transform";
    AlphaBetaToDq0Transform transform(name, root);
    ASSERT_EQ(transform.getName(), name);

    auto serialized = transform.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "AlphaBetaToDq0Transform");
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

TEST_F(AlphaBetaToDq0TransformTest, BasicTest)
{
    RootComponent           root;
    std::string_view        name = "AlphaBetaToDq0Transform3";
    AlphaBetaToDq0Transform transform(name, root);

    double i_alpha     = 1.0;
    double i_beta      = -0.5;
    double i_zero      = 0.0;
    double theta       = M_PI / 6;   // 30 degrees in radians
    bool   a_alignment = true;
    auto [d, q, zero]  = transform.transform(i_alpha, i_beta, i_zero, theta, a_alignment);

    // Expected values calculation
    const double cos_theta = std::cos(theta);
    const double sin_theta = std::sin(theta);

    double expected_d    = i_alpha * cos_theta + i_beta * sin_theta;
    double expected_q    = -i_alpha * sin_theta + i_beta * cos_theta;
    double expected_zero = i_zero;

    EXPECT_NEAR(d, expected_d, 1e-4);
    EXPECT_NEAR(q, expected_q, 1e-4);
    EXPECT_NEAR(zero, expected_zero, 1e-4);
}

TEST_F(AlphaBetaToDq0TransformTest, ZeroAngleTest)
{
    RootComponent           root;
    std::string_view        name = "AlphaBetaToDq0Transform4";
    AlphaBetaToDq0Transform transform(name, root);

    double i_alpha     = 1.0;
    double i_beta      = -0.5;
    double i_zero      = -0.5;
    double theta       = 0.0;
    bool   a_alignment = true;
    auto [d, q, zero]  = transform.transform(i_alpha, i_beta, i_zero, theta);

    // Expected values calculation
    const double cos_theta = std::cos(theta);
    const double sin_theta = std::sin(theta);

    double expected_d    = i_alpha * cos_theta + i_beta * sin_theta;
    double expected_q    = -i_alpha * sin_theta + i_beta * cos_theta;
    double expected_zero = i_zero;

    EXPECT_NEAR(d, expected_d, 1e-4);
    EXPECT_NEAR(q, expected_q, 1e-4);
    EXPECT_NEAR(zero, expected_zero, 1e-4);
}

TEST_F(AlphaBetaToDq0TransformTest, ZeroAngle90degreesOffsetTest)
{
    RootComponent           root;
    std::string_view        name = "AlphaBetaToDq0Transform5";
    AlphaBetaToDq0Transform transform(name, root);

    const double i_alpha     = 1.0;
    const double i_beta      = -0.5;
    const double i_zero      = -0.5;
    const double theta       = 0.0;
    bool         a_alignment = false;
    auto [d, q, zero]        = transform.transform(i_alpha, i_beta, i_zero, theta, a_alignment);

    // Expected values calculation
    const double cos_theta              = std::cos(theta);
    const double cos_theta_two_thirds   = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_m_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta              = std::sin(theta);
    const double sin_theta_two_thirds   = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_m_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    // inverse clarke:
    const double u_a = i_alpha + i_zero;
    const double u_b = (-i_alpha + sqrt(3) * i_beta) / 2.0 + i_zero;
    const double u_c = -(i_alpha + sqrt(3) * i_beta) / 2.0 + i_zero;

    double expected_d    = (2.0 / 3.0) * (u_a * sin_theta + u_b * sin_theta_m_two_thirds + u_c * sin_theta_two_thirds);
    double expected_q    = (2.0 / 3.0) * (u_a * cos_theta + u_b * cos_theta_m_two_thirds + u_c * cos_theta_two_thirds);
    double expected_zero = (1.0 / 3.0) * (u_a + u_b + u_c);

    EXPECT_NEAR(d, expected_d, 1e-4);
    EXPECT_NEAR(q, expected_q, 1e-4);
    EXPECT_NEAR(zero, expected_zero, 1e-4);
}

//! Tests interacting with transform method of ParkTransform component, validation against simulink
TEST_F(AlphaBetaToDq0TransformTest, SimulinkConsistencyAaxisAlignment)
{
    RootComponent           root;
    std::string_view        name = "AlphaBetaToDq0Transform6";
    AlphaBetaToDq0Transform transform(name, root);

    // the input files are randomly generated numbers
    std::filesystem::path abz_path   = "components/inputs/alpha-beta-zero_sin_120degrees.csv";
    std::filesystem::path theta_path = "components/inputs/park_theta_0_20.csv";
    std::filesystem::path dq0_path   = "components/inputs/alpha-beta-zero_to_dq0_sin_120degrees_theta_0_20.csv";

    fgc4::utils::test::ReadCSV<3> dq0_file(dq0_path);
    fgc4::utils::test::ReadCSV<2> theta_file(theta_path);
    fgc4::utils::test::ReadCSV<3> abz_file(abz_path);

    while (!abz_file.eof() && !theta_file.eof() && !dq0_file.eof())
    {
        const auto dq0_line   = dq0_file.readLine();
        const auto theta_line = theta_file.readLine();
        const auto abz_line   = abz_file.readLine();

        if (abz_line && theta_line && dq0_line)
        {
            const auto [matlab_d, matlab_q, matlab_zero] = dq0_line.value();
            const auto [_, theta]                        = theta_line.value();
            const auto [f_alpha, f_beta, f_zero]         = abz_line.value();

            // validation
            const auto [d, q, zero]  = transform.transform(f_alpha, f_beta, f_zero, theta);
            const auto relative_d    = (matlab_d - d);
            const auto relative_q    = (matlab_q - q);
            const auto relative_zero = (matlab_zero - zero);

            ASSERT_NEAR(relative_d, 0.0, 1e-6);      // at least 1e-6 relative precision
            ASSERT_NEAR(relative_q, 0.0, 1e-6);      // at least 1e-6 relative precision
            ASSERT_NEAR(relative_zero, 0.0, 1e-6);   // at least 1e-6 relative precision
        }
    }
}

//! Tests interacting with transform method of ParkTransform component, validation against simulink
TEST_F(AlphaBetaToDq0TransformTest, SimulinkConsistencyAaxisNotAligned)
{
    RootComponent           root;
    std::string_view        name = "AlphaBetaToDq0Transform7";
    AlphaBetaToDq0Transform transform(name, root);

    // the input files are randomly generated numbers
    std::filesystem::path abz_path   = "components/inputs/alpha-beta-zero_sin_120degrees.csv";
    std::filesystem::path theta_path = "components/inputs/park_theta_0_20.csv";
    std::filesystem::path dq0_path
        = "components/inputs/alpha-beta-zero_to_dq0_sin_120degrees_theta_0_20_a_notaligned.csv";

    fgc4::utils::test::ReadCSV<3> dq0_file(dq0_path);
    fgc4::utils::test::ReadCSV<2> theta_file(theta_path);
    fgc4::utils::test::ReadCSV<3> abz_file(abz_path);

    while (!abz_file.eof() && !theta_file.eof() && !dq0_file.eof())
    {
        const auto dq0_line   = dq0_file.readLine();
        const auto theta_line = theta_file.readLine();
        const auto abz_line   = abz_file.readLine();

        if (abz_line && theta_line && dq0_line)
        {
            const auto [matlab_d, matlab_q, matlab_zero] = dq0_line.value();
            const auto [_, theta]                        = theta_line.value();
            const auto [f_alpha, f_beta, f_zero]         = abz_line.value();

            // validation
            const auto [d, q, zero]  = transform.transform(f_alpha, f_beta, f_zero, theta, false);
            const auto relative_d    = (matlab_d - d);
            const auto relative_q    = (matlab_q - q);
            const auto relative_zero = (matlab_zero - zero);

            ASSERT_NEAR(relative_d, 0.0, 1e-6);      // at least 1e-6 relative precision
            ASSERT_NEAR(relative_q, 0.0, 1e-6);      // at least 1e-6 relative precision
            ASSERT_NEAR(relative_zero, 0.0, 1e-6);   // at least 1e-6 relative precision
        }
    }
}