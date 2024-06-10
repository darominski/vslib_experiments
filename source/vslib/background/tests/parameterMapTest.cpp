//! @file
//! @brief File with unit tests of ParameterMap background-task class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "component.h"
#include "json/json.hpp"
#include "messageQueue.h"
#include "parameterMap.h"

using namespace vslib;

class ParameterMapTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks that a ParameterMap object can be constructed
TEST_F(ParameterMapTest, ParameterMapDefaultConstruction)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> buffer{};
    Component                       root_component("test_type", "test_name", nullptr);
    ASSERT_NO_THROW(ParameterMap(buffer.data(), queue_size, root_component));
}

//! Checks that a ParameterMap can upload a parameter map to the queue for a mock component
TEST_F(ParameterMapTest, ParameterMapUploadSimpleMap)
{
    constexpr size_t                queue_size = 1000;   // 1000 bytes
    std::array<uint8_t, queue_size> buffer{};
    Component                       root_component("type", "name", nullptr);
    ParameterMap                    parameter_map(buffer.data(), queue_size, root_component);

    auto read_queue
        = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>((uint8_t*)buffer.data(), queue_size);
    std::array<uint8_t, queue_size> read_buffer;

    ASSERT_NO_THROW(parameter_map.uploadParameterMap());

    auto message = read_queue.read(read_buffer);
    EXPECT_TRUE(message.has_value());
    auto json_object = nlohmann::json::parse(message.value().begin(), message.value().end());
    EXPECT_EQ(
        json_object.dump(),
        "[{\"version\":[0,1,0]},{\"components\":[],\"name\":\"name\",\"parameters\":[],\"type\":\"type\"}]"
    );
}
