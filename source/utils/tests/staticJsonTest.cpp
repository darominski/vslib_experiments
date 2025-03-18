//! @file
//! @brief File with unit tests for StaticJson objects and its factory.
//! @author Dominik Arominski

#include <gtest/gtest.h>
#include <string>

#include "constants.hpp"
#include "staticJson.hpp"

using namespace fgc4::utils;

class StaticJsonTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks for basic JSON allocation
TEST(StaticJsonTest, BasicJsonObject)
{
    StaticJson json = StaticJsonFactory::getJsonObject();
    json            = {{"name", "PID"}, {"value", 30}};
    // Verify the structure
    EXPECT_TRUE(json.is_object());
    EXPECT_EQ(json["name"], "PID");
    EXPECT_EQ(json["value"], 30);
}

//! Checks for array JSON allocation
TEST(StaticJsonTest, JsonArray)
{
    StaticJson json = StaticJsonFactory::getJsonObject();
    json["values"]  = {100, 90, 85};

    // Verify the structure and contents
    EXPECT_TRUE(json.is_object());
    EXPECT_TRUE(json["values"].is_array());
    EXPECT_EQ(json["values"][0], 100);
    EXPECT_EQ(json["values"][1], 90);
    EXPECT_EQ(json["values"][2], 85);
}

//! Checks allocation of hierarchical JSON object
TEST(StaticJsonTest, NestedJsonObject)
{
    const std::string component_name = "PID";
    const std::string p_name         = "p";
    const std::string i_name         = "i";
    const std::string d_name         = "d";
    const double      p_value        = 1.0;
    const double      i_value        = 2.0;
    const double      d_value        = 3.0;

    StaticJson json = StaticJsonFactory::getJsonObject();
    json["name"]    = component_name;
    json["parameters"]
        = {{{"name", p_name}, {"value", p_value}},
           {{"name", i_name}, {"value", i_value}},
           {{"name", d_name}, {"value", d_value}}};

    // Verify the structure and contents
    EXPECT_TRUE(json.is_object());
    EXPECT_EQ(json["name"], component_name);
    EXPECT_EQ(json["parameters"][0]["name"], p_name);
    EXPECT_EQ(json["parameters"][0]["value"], p_value);
    EXPECT_EQ(json["parameters"][1]["name"], i_name);
    EXPECT_EQ(json["parameters"][1]["value"], i_value);
    EXPECT_EQ(json["parameters"][2]["name"], d_name);
    EXPECT_EQ(json["parameters"][2]["value"], d_value);
}

//! Checks for serialization and deserialization
TEST(StaticJsonTest, SerializationDeserialization)
{
    StaticJson json = StaticJsonFactory::getJsonObject();

    double p_value = 3.14159;
    json["name"]   = "P";
    json["value"]  = p_value;

    // Serialize to a string
    std::string json_string = json.dump();

    // Deserialize back to a JSON object
    StaticJson deserialized_json = StaticJson::parse(json_string);

    // Verify the structure and contents
    EXPECT_TRUE(deserialized_json.is_object());
    EXPECT_EQ(deserialized_json["name"], "P");
    EXPECT_EQ(deserialized_json["value"], p_value);
}

//! Checks that the memory pool is cleaned up when requested
TEST(StaticJsonTest, FreshJsonObject)
{
    StaticJson json1 = StaticJsonFactory::getJsonObject();
    json1["name"]    = "P";

    StaticJson json2 = StaticJsonFactory::getJsonObject();
    EXPECT_FALSE(json2.contains("name"));   // json2 should be fresh without "name" key
}

// test for allocating too large JSON objects does not behave as intended,
// as the allocator sees nlohmann objects' size as 1-2 bytes, regardless its internal size