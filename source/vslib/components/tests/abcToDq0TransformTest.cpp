//! @file
//! @brief File with unit tests for AbcToDq0Transform component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "abcToDq0Transform.hpp"
#include "csv.hpp"
#include "mockRoot.hpp"

using namespace vslib;
using namespace csv;

class AbcToDq0TransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of AbcToDq0Transform component
TEST_F(AbcToDq0TransformTest, Construction)
{
    MockRoot          root;
    std::string_view  name = "park1";
    AbcToDq0Transform park(name, root);
    ASSERT_EQ(park.getName(), "park1");

    auto serialized = park.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "AbcToDq0Transform");
    EXPECT_EQ(serialized["components"].size(), 2);
    EXPECT_EQ(
        serialized["components"].dump(),
        "[{\"name\":\"abc_to_alphabeta\",\"type\":\"AbcToAlphaBetaTransform\",\"parameters\":[],\"components\":[]},{"
        "\"name\":\"alphabeta_to_dq0\",\"type\":\"AlphaBetaToDq0Transform\",\"parameters\":[],\"components\":[{"
        "\"name\":"
        "\"sin\",\"type\":\"SinLookupTable\",\"parameters\":[],\"components\":[{\"name\":\"data\",\"type\":"
        "\"LookupTable\",\"parameters\":[],\"components\":[]}]},{\"name\":\"cos\",\"type\":\"CosLookupTable\","
        "\"parameters\":[],\"components\":[{\"name\":\"data\",\"type\":\"LookupTable\",\"parameters\":[],"
        "\"components\":[]}]}]}]"
    );
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

TEST_F(AbcToDq0TransformTest, BasicTest)
{
    MockRoot          root;
    std::string_view  name = "park2";
    AbcToDq0Transform park(name, root);

    double i_a        = 1.0;
    double i_b        = -0.5;
    double i_c        = -0.5;
    double theta      = M_PI / 6;   // 30 degrees in radians
    auto [d, q, zero] = park.transform(i_a, i_b, i_c, theta);

    // Expected values calculation
    const double cos_theta              = std::cos(theta);
    const double cos_theta_two_thirds   = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_m_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta              = std::sin(theta);
    const double sin_theta_two_thirds   = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_m_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    double expected_d    = (2.0 / 3.0) * (i_a * sin_theta + i_b * sin_theta_m_two_thirds + i_c * sin_theta_two_thirds);
    double expected_q    = (2.0 / 3.0) * (i_a * cos_theta + i_b * cos_theta_m_two_thirds + i_c * cos_theta_two_thirds);
    double expected_zero = (1.0 / 3.0) * (i_a + i_b + i_c);

    EXPECT_NEAR(d, expected_d, 1e-4);
    EXPECT_NEAR(q, expected_q, 1e-4);
    EXPECT_NEAR(zero, expected_zero, 1e-4);
}

TEST_F(AbcToDq0TransformTest, ZeroAngleTest)
{
    MockRoot          root;
    std::string_view  name = "park3";
    AbcToDq0Transform park(name, root);

    double i_a        = 1.0;
    double i_b        = -0.5;
    double i_c        = -0.5;
    double theta      = 0.0;
    auto [d, q, zero] = park.transform(i_a, i_b, i_c, theta);

    // Expected values calculation
    const double cos_theta              = std::cos(theta);
    const double cos_theta_two_thirds   = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_m_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta              = std::sin(theta);
    const double sin_theta_two_thirds   = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_m_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    double expected_d    = (2.0 / 3.0) * (i_a * sin_theta + i_b * sin_theta_m_two_thirds + i_c * sin_theta_two_thirds);
    double expected_q    = (2.0 / 3.0) * (i_a * cos_theta + i_b * cos_theta_m_two_thirds + i_c * cos_theta_two_thirds);
    double expected_zero = (1.0 / 3.0) * (i_a + i_b + i_c);

    EXPECT_NEAR(d, expected_d, 1e-5);
    EXPECT_NEAR(q, expected_q, 1e-5);
    EXPECT_NEAR(zero, expected_zero, 1e-5);
}

TEST_F(AbcToDq0TransformTest, ZeroAngle90degreesOffsetTest)
{
    MockRoot          root;
    std::string_view  name = "park3";
    AbcToDq0Transform park(name, root, 10'000);

    const double i_a    = 1.0;
    const double i_b    = -0.5;
    const double i_c    = -0.5;
    const double theta  = std::numbers::pi / 12.0;
    const double offset = M_PI / 2.0;
    auto [d, q, zero]   = park.transform(i_a, i_b, i_c, theta, offset);

    // Expected values calculation
    const double cos_theta              = std::cos(theta);
    const double cos_theta_two_thirds   = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_m_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta              = std::sin(theta);
    const double sin_theta_two_thirds   = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_m_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    double expected_d    = (2.0 / 3.0) * (i_a * cos_theta + i_b * cos_theta_m_two_thirds + i_c * cos_theta_two_thirds);
    double expected_q    = (2.0 / 3.0) * (-i_a * sin_theta - i_b * sin_theta_m_two_thirds - i_c * sin_theta_two_thirds);
    double expected_zero = (1.0 / 3.0) * (i_a + i_b + i_c);

    EXPECT_NEAR(d, expected_d, 1e-6);
    EXPECT_NEAR(q, expected_q, 1e-6);
    EXPECT_NEAR(zero, expected_zero, 1e-6);
}

TEST_F(AbcToDq0TransformTest, NinetyDegreesTest)
{
    MockRoot          root;
    std::string_view  name = "park4";
    AbcToDq0Transform park(name, root);

    double i_a        = 1.0;
    double i_b        = -0.5;
    double i_c        = -0.5;
    double theta      = M_PI / 2;   // 90 degrees in radians
    auto [d, q, zero] = park.transform(i_a, i_b, i_c, theta);

    // Expected values calculation
    const double cos_theta              = std::cos(theta);
    const double cos_theta_two_thirds   = std::cos(theta + 2.0 * M_PI / 3.0);
    const double cos_theta_m_two_thirds = std::cos(theta - 2.0 * M_PI / 3.0);

    const double sin_theta              = std::sin(theta);
    const double sin_theta_two_thirds   = std::sin(theta + 2.0 * M_PI / 3.0);
    const double sin_theta_m_two_thirds = std::sin(theta - 2.0 * M_PI / 3.0);

    double expected_d    = (2.0 / 3.0) * (i_a * sin_theta + i_b * sin_theta_m_two_thirds + i_c * sin_theta_two_thirds);
    double expected_q    = (2.0 / 3.0) * (i_a * cos_theta + i_b * cos_theta_m_two_thirds + i_c * cos_theta_two_thirds);
    double expected_zero = (1.0 / 3.0) * (i_a + i_b + i_c);

    EXPECT_NEAR(d, expected_d, 1e-5);
    EXPECT_NEAR(q, expected_q, 1e-5);
    EXPECT_NEAR(zero, expected_zero, 1e-5);
}

//! Tests interacting with transform method of AbcToDq0Transform component, validation against simulink
TEST_F(AbcToDq0TransformTest, BasicSimulinkConsistency)
{
    MockRoot          root;
    std::string_view  name = "park5";
    AbcToDq0Transform park(name, root, 10'000);

    // the input files are randomly generated numbers
    std::filesystem::path abc_path   = "components/inputs/park_abc_sin_120degrees.csv";
    std::filesystem::path theta_path = "components/inputs/park_theta_0_20.csv";
    std::filesystem::path dq0_path   = "components/inputs/park_dq0_sin_120degrees_theta_0_20.csv";

    CSVReader abc_file(abc_path.c_str());
    CSVReader dq0_file(dq0_path.c_str());
    CSVReader theta_file(theta_path.c_str());

    auto abc_line   = abc_file.begin();
    auto dq0_line   = dq0_file.begin();
    auto theta_line = theta_file.begin();

    while (abc_line != abc_file.end() && dq0_line != dq0_file.end() && theta_line != theta_file.end())
    {
        const auto a = (*abc_line)[1].get<double>();
        const auto b = (*abc_line)[2].get<double>();
        const auto c = (*abc_line)[3].get<double>();

        const auto theta = (*theta_line)[1].get<double>();

        const auto matlab_d    = (*dq0_line)[0].get<double>();
        const auto matlab_q    = (*dq0_line)[1].get<double>();
        const auto matlab_zero = (*dq0_line)[2].get<double>();

        // validation
        const auto [d, q, zero]  = park.transform(a, b, c, theta);
        const auto relative_d    = (matlab_d - d);
        const auto relative_q    = (matlab_q - q);
        const auto relative_zero = (matlab_zero - zero);

        ASSERT_NEAR(relative_d, 0.0, 1e-6);      // at least 1e-6 relative precision
        ASSERT_NEAR(relative_q, 0.0, 1e-6);      // at least 1e-6 relative precision
        ASSERT_NEAR(relative_zero, 0.0, 1e-6);   // at least 1e-6 relative precision

        ++abc_line;
        ++dq0_line;
        ++theta_line;
    }
}

//! Tests interacting with transform method of AbcToDq0Transform component, validation against simulink and SVC measured
//! data
TEST_F(AbcToDq0TransformTest, SVCTransform)
{
    MockRoot          root;
    std::string_view  name = "park5";
    AbcToDq0Transform park(name, root);

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
        const auto a = (*abc_line)[1].get<double>();
        const auto b = (*abc_line)[2].get<double>();
        const auto c = (*abc_line)[3].get<double>();

        const auto theta = (*theta_line)[1].get<double>();

        const auto matlab_d    = (*dq0_line)[0].get<double>();
        const auto matlab_q    = (*dq0_line)[1].get<double>();
        const auto matlab_zero = (*dq0_line)[2].get<double>();

        // validation
        const auto [d, q, zero]  = park.transform(a, b, c, theta);
        const auto relative_d    = (matlab_d - d);
        const auto relative_q    = (matlab_q - q);
        const auto relative_zero = (matlab_zero - zero);

        ASSERT_NEAR(relative_d, 0.0, 1e-6);      // at least 1e-6 relative precision
        ASSERT_NEAR(relative_q, 0.0, 1e-6);      // at least 1e-6 relative precision
        ASSERT_NEAR(relative_zero, 0.0, 1e-6);   // at least 1e-6 relative precision

        ++abc_line;
        ++dq0_line;
        ++theta_line;
    }
}
