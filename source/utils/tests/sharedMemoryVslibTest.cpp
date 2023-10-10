//! @file
//! @brief File with vslib shared memory unit tests.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "sharedMemoryVslib.h"

using namespace vslib;

class SharedMemoryVslibTest : public ::testing::Test
{
  protected:
    SharedMemory shared_memory;

    void SetUp() override
    {
        // Optionally set up the shared memory instance before each test
        std::memset(&shared_memory, 0, sizeof(SharedMemory));   // Initialize to zeros
    }
};

TEST_F(SharedMemoryVslibTest, WriteJsonToSharedMemory)
{
    nlohmann::json json_data = {{"key", "value"}};

    writeJsonToSharedMemory(json_data, &shared_memory);

    EXPECT_EQ(shared_memory.message_length, json_data.dump().size());
}

TEST_F(SharedMemoryVslibTest, ReadJsonFromSharedMemory)
{
    // Set up shared memory with JSON data
    nlohmann::json json_data = {{"key", "value"}};
    writeJsonToSharedMemory(json_data, &shared_memory);

    nlohmann::json read_data = readJsonFromSharedMemory(&shared_memory);

    // Add assertions to verify the state of read data
    EXPECT_EQ(read_data, json_data);
}

TEST_F(SharedMemoryVslibTest, WriteTooLongJsonToSharedMemory)
{
    nlohmann::json json_data;
    for (size_t index = 0; index < 1024 * 128; index++)
    {
        json_data.push_back({{"key", "value"}});
    }

    EXPECT_THROW(writeJsonToSharedMemory(json_data, &shared_memory), std::bad_alloc);
}