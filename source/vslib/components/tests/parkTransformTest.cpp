//! @file
//! @brief File with unit tests for ParkTransform component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "parkTransform.h"

using namespace vslib;

class ParkTransformTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of ParkTransform component
TEST_F(ParkTransformTest, ParkTransformConstructionTest)
{
    std::string_view name = "park1";
    ParkTransform    park(name, nullptr);
    ASSERT_EQ(park.getName(), "park1");

    auto serialized = park.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "ParkTransform");
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

//! Tests custom construction of ParkTransform component
TEST_F(ParkTransformTest, ParkTransformNonDefaultConstructionTest)
{
    std::string_view name = "park2";
    ParkTransform    park(name, nullptr, 10000);
    ASSERT_EQ(park.getName(), name);

    auto serialized = park.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "ParkTransform");
    EXPECT_EQ(serialized["components"].size(), 2);
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests interacting with transform method of ParkTransform component, with trivial input
TEST_F(ParkTransformTest, ParkTransformTrivialInput)
{
    std::string_view name = "park2";
    ParkTransform    park(name, nullptr, 10000);

    auto [d, q] = park.transform(1, -0.5, 0, 0);
    EXPECT_EQ(d, 1.0);
    EXPECT_EQ(q, 0.0);
}

//! Tests interacting with transform method of ParkTransform component, validation against simulink
TEST_F(ParkTransformTest, ParkTransformSimulinkConsistency)
{
    std::string_view name = "park2";
    ParkTransform    park(name, nullptr, 10000);

    auto [d, q] = park.transform(1, -0.5, 0, 0);
    EXPECT_EQ(d, 1.0);
    EXPECT_EQ(q, 0.0);

    // the input files are randomly generated numbers
    std::filesystem::path abc_path   = "components/inputs/abc.csv";
    std::filesystem::path theta_path = "components/inputs/theta.csv";
    std::filesystem::path park_path  = "components/inputs/park.csv";

    std::ifstream abc_file(abc_path);
    std::ifstream theta_file(theta_path);
    std::ifstream park_file(park_path);

    ASSERT_TRUE(abc_file.is_open());
    ASSERT_TRUE(theta_file.is_open());
    ASSERT_TRUE(park_file.is_open());

    std::string abc_line;
    std::string theta_line;
    std::string park_line;

    while (getline(abc_file, abc_line) && getline(theta_file, theta_line) && getline(park_file, park_line))
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

        std::string       theta_str;
        std::stringstream ss_theta(theta_line);
        std::getline(ss_theta, timestamp, ',');
        std::getline(ss_theta, theta_str, ',');
        const auto theta = std::stod(theta_str);

        // matlab outputs
        std::string       d_str, q_str;
        std::stringstream ss_park(park_line);
        std::getline(ss_park, d_str, ',');
        const auto matlab_d = std::stod(d_str);

        std::getline(ss_park, q_str, ',');
        const auto matlab_q = std::stod(q_str);

        // validation
        const auto [d, q]     = park.transform(a, b, c, theta);
        const auto relative_d = (matlab_d - d) / matlab_d;
        const auto relative_q = (matlab_q - q) / matlab_q;

        EXPECT_NEAR(relative_d, 0.0, 1e-6);   // at least 1e-6 relative precision
        EXPECT_NEAR(relative_q, 0.0, 1e-6);   // at least 1e-6 relative precision
    }
    abc_file.close();
    theta_file.close();
    park_file.close();
}