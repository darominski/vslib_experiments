//! @file
//! @brief File with unit tests for ClarkeParkTransformTest component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "clarkeParkTransform.h"

using namespace vslib;

class ClarkeParkTransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of ClarkeParkTransform component
TEST_F(ClarkeParkTransformTest, Construction)
{
    std::string_view    name = "clarkePark";
    ClarkeParkTransform clarkePark(name, nullptr);
    ASSERT_EQ(clarkePark.getName(), name);

    auto serialized = clarkePark.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "ClarkeParkTransform");
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

//! Tests custom construction of ClarkeParkTransform component
TEST_F(ClarkeParkTransformTest, NonDefaultConstruction)
{
    std::string_view    name = "clarkePark2";
    ClarkeParkTransform clarkePark(name, nullptr, 10000);
    ASSERT_EQ(clarkePark.getName(), name);

    auto serialized = clarkePark.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "ClarkeParkTransform");
    EXPECT_EQ(serialized["components"].size(), 2);
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

TEST_F(ClarkeParkTransformTest, BasicTest)
{
    std::string_view    name = "clarkePark3";
    ClarkeParkTransform clarkePark(name, nullptr);

    double i_alpha     = 1.0;
    double i_beta      = -0.5;
    double i_zero      = 0.0;
    double theta       = M_PI / 6;   // 30 degrees in radians
    bool   a_alignment = true;
    auto [d, q, zero]  = clarkePark.transform(i_alpha, i_beta, i_zero, theta, a_alignment);

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

TEST_F(ClarkeParkTransformTest, ZeroAngleTest)
{
    std::string_view    name = "clarkePark4";
    ClarkeParkTransform clarkePark(name, nullptr);

    double i_alpha     = 1.0;
    double i_beta      = -0.5;
    double i_zero      = -0.5;
    double theta       = 0.0;
    bool   a_alignment = true;
    auto [d, q, zero]  = clarkePark.transform(i_alpha, i_beta, i_zero, theta);

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

TEST_F(ClarkeParkTransformTest, ZeroAngle90degreesOffsetTest)
{
    std::string_view    name = "clarkePark5";
    ClarkeParkTransform clarkePark(name, nullptr);

    const double i_alpha     = 1.0;
    const double i_beta      = -0.5;
    const double i_zero      = -0.5;
    const double theta       = 0.0;
    bool         a_alignment = false;
    auto [d, q, zero]        = clarkePark.transform(i_alpha, i_beta, i_zero, theta, a_alignment);

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
TEST_F(ClarkeParkTransformTest, SimulinkConsistencyAaxisAlignment)
{
    std::string_view    name = "clarkePar6";
    ClarkeParkTransform clarkePark(name, nullptr, 10000);

    // the input files are randomly generated numbers
    std::filesystem::path abz_path    = "components/inputs/clarkePark_alpha-beta-zero_sin_120degrees.csv";
    std::filesystem::path theta_path  = "components/inputs/park_theta_0_20.csv";
    std::filesystem::path output_path = "components/inputs/clarkePark_dq0_sin_120degrees_theta_0_20.csv";

    std::ifstream abz_file(abz_path);
    std::ifstream theta_file(theta_path);
    std::ifstream output_file(output_path);

    ASSERT_TRUE(abz_file.is_open());
    ASSERT_TRUE(theta_file.is_open());
    ASSERT_TRUE(output_file.is_open());

    std::string abz_line;
    std::string theta_line;
    std::string output_line;

    while (getline(abz_file, abz_line) && getline(theta_file, theta_line) && getline(output_file, output_line))
    {
        // matlab inputs:
        std::stringstream ss(abz_line);
        std::string       timestamp, a_str, b_str, z_str;

        // Get the f_alpha value
        std::getline(ss, a_str, ',');
        const auto f_alpha = std::stod(a_str);

        // Get the f_beta value
        std::getline(ss, b_str, ',');
        const auto f_beta = std::stod(b_str);

        // Get the f_zero value
        std::getline(ss, z_str, ',');
        const auto f_zero = std::stod(z_str);

        std::string       theta_str;
        std::stringstream ss_theta(theta_line);
        std::getline(ss_theta, timestamp, ',');
        std::getline(ss_theta, theta_str, ',');
        const auto theta = std::stod(theta_str);

        // matlab outputs
        std::string       d_str, q_str, zero_str;
        std::stringstream ss_output(output_line);
        std::getline(ss_output, d_str, ',');
        const auto matlab_d = std::stod(d_str);

        std::getline(ss_output, q_str, ',');
        const auto matlab_q = std::stod(q_str);

        std::getline(ss_output, zero_str, ',');
        const auto matlab_zero = std::stod(zero_str);

        // validation
        const auto [d, q, zero]  = clarkePark.transform(f_alpha, f_beta, f_zero, theta);
        const auto relative_d    = (matlab_d - d);
        const auto relative_q    = (matlab_q - q);
        const auto relative_zero = (matlab_zero - zero);

        EXPECT_NEAR(relative_d, 0.0, 1e-6);      // at least 1e-6 relative precision
        EXPECT_NEAR(relative_q, 0.0, 1e-6);      // at least 1e-6 relative precision
        EXPECT_NEAR(relative_zero, 0.0, 1e-6);   // at least 1e-6 relative precision
    }
    abz_file.close();
    theta_file.close();
    output_file.close();
}


//! Tests interacting with transform method of ParkTransform component, validation against simulink
TEST_F(ClarkeParkTransformTest, SimulinkConsistencyAaxisNotAligned)
{
    std::string_view    name = "clarkePar7";
    ClarkeParkTransform clarkePark(name, nullptr, 10000);

    // the input files are randomly generated numbers
    std::filesystem::path abz_path    = "components/inputs/clarkePark_alpha-beta-zero_sin_120degrees.csv";
    std::filesystem::path theta_path  = "components/inputs/park_theta_0_20.csv";
    std::filesystem::path output_path = "components/inputs/clarkePark_dq0_sin_120degrees_theta_0_20_a_notaligned.csv";

    std::ifstream abz_file(abz_path);
    std::ifstream theta_file(theta_path);
    std::ifstream output_file(output_path);

    ASSERT_TRUE(abz_file.is_open());
    ASSERT_TRUE(theta_file.is_open());
    ASSERT_TRUE(output_file.is_open());

    std::string abz_line;
    std::string theta_line;
    std::string output_line;

    while (getline(abz_file, abz_line) && getline(theta_file, theta_line) && getline(output_file, output_line))
    {
        // matlab inputs:
        std::stringstream ss(abz_line);
        std::string       timestamp, a_str, b_str, z_str;

        // Get the f_alpha value
        std::getline(ss, a_str, ',');
        const auto f_alpha = std::stod(a_str);

        // Get the f_beta value
        std::getline(ss, b_str, ',');
        const auto f_beta = std::stod(b_str);

        // Get the f_zero value
        std::getline(ss, z_str, ',');
        const auto f_zero = std::stod(z_str);

        std::string       theta_str;
        std::stringstream ss_theta(theta_line);
        std::getline(ss_theta, timestamp, ',');
        std::getline(ss_theta, theta_str, ',');
        const auto theta = std::stod(theta_str);

        // matlab outputs
        std::string       d_str, q_str, zero_str;
        std::stringstream ss_output(output_line);
        std::getline(ss_output, d_str, ',');
        const auto matlab_d = std::stod(d_str);

        std::getline(ss_output, q_str, ',');
        const auto matlab_q = std::stod(q_str);

        std::getline(ss_output, zero_str, ',');
        const auto matlab_zero = std::stod(zero_str);

        // validation
        const auto [d, q, zero]  = clarkePark.transform(f_alpha, f_beta, f_zero, theta, false);
        const auto relative_d    = (matlab_d - d);
        const auto relative_q    = (matlab_q - q);
        const auto relative_zero = (matlab_zero - zero);

        ASSERT_NEAR(relative_d, 0.0, 1e-6);      // at least 1e-6 relative precision
        EXPECT_NEAR(relative_q, 0.0, 1e-6);      // at least 1e-6 relative precision
        EXPECT_NEAR(relative_zero, 0.0, 1e-6);   // at least 1e-6 relative precision
    }
    abz_file.close();
    theta_file.close();
    output_file.close();
}