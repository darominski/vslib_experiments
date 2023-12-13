//! @file
//! @brief File containing tests of the background task functions.
//! @author Dominik Arominski

#include "gtest/gtest.h"

// Include necessary headers for your implementation
#include "background.h"
#include "bufferSwitch.h"
#include "parameter.h"
#include "parameterRegistry.h"
#include "staticJson.h"
#include "warningMessage.h"

using namespace fgc4::utils;
using namespace vslib;
using namespace vslib::backgroundTask;

// Test fixture class
class BackgroundTaskTest : public testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        ParameterRegistry::instance().clearRegistry();
        ComponentRegistry::instance().clearRegistry();
    }
};

// Mock component for testing
class MockComponent : public vslib::Component
{
  public:
    MockComponent()
        : Component("MockType", "MockName", nullptr)
    {
    }
};

// Tests initialization of shared memory to a known state, requires running on baree m,
TEST_F(BackgroundTaskTest, InitializeSharedMemory)
{
    SharedMemory shared_memory;
    vslib::backgroundTask::initializeMemory(&shared_memory);
    EXPECT_EQ(shared_memory.acknowledged_counter, 0);
    EXPECT_EQ(shared_memory.transmitted_counter, 0);
    EXPECT_EQ(shared_memory.message_length, 0);
    for (auto const& element : shared_memory.json_buffer)
    {
        EXPECT_EQ(element, std::byte());
    }
}

// Tests triggering creation of parameter map and its uploading to the shared memory structure
TEST_F(BackgroundTaskTest, UploadParameterMap)
{
    SharedMemory shared_memory;
    vslib::backgroundTask::initializeMemory(&shared_memory);
    MockComponent      component;
    Parameter<int32_t> parameter(component, "parameter");
    uploadParameterMap(&shared_memory);
    EXPECT_EQ(shared_memory.message_length, 128);
    nlohmann::json parameter_map = nlohmann::json::parse(
        shared_memory.json_buffer.begin(), shared_memory.json_buffer.begin() + shared_memory.message_length
    );
    EXPECT_NE(parameter_map, nlohmann::json::object());
    EXPECT_TRUE(parameter_map.is_array());
    EXPECT_TRUE(parameter_map[0].is_object());
    EXPECT_TRUE(parameter_map[0].contains("components"));
    EXPECT_TRUE(parameter_map[0].contains("parameters"));
    EXPECT_TRUE(parameter_map[0].contains("name"));
    EXPECT_TRUE(parameter_map[0].contains("type"));
    EXPECT_TRUE(parameter_map[0]["parameters"][0].contains("length"));
    EXPECT_TRUE(parameter_map[0]["parameters"][0].contains("name"));
    EXPECT_TRUE(parameter_map[0]["parameters"][0].contains("type"));
    EXPECT_TRUE(parameter_map[0]["parameters"][0].contains("value"));
    EXPECT_EQ(parameter_map[0]["components"], nlohmann::json::array());
    EXPECT_EQ(parameter_map[0]["name"], "MockName");
    EXPECT_EQ(parameter_map[0]["type"], "MockType");
    EXPECT_EQ(parameter_map[0]["parameters"][0]["length"], 1);
    EXPECT_EQ(parameter_map[0]["parameters"][0]["value"], nlohmann::json::object());
    EXPECT_EQ(parameter_map[0]["parameters"][0]["name"], "parameter");
    EXPECT_EQ(parameter_map[0]["parameters"][0]["type"], "Int32");
}

// Tests validation of the incoming json command
TEST_F(BackgroundTaskTest, ValidateJsonCommand)
{
    StaticJson command_no_name  = {{"value", 1.0}};
    StaticJson command_no_value = {{"name", "p"}};
    StaticJson command_valid    = {{"name", "p"}, {"value", 1}};

    EXPECT_EQ(validateJsonCommand(command_no_name), false);
    EXPECT_EQ(validateJsonCommand(command_no_value), false);
    EXPECT_EQ(validateJsonCommand(command_valid), true);
}

// Tests execution workflow of JSON command
TEST_F(BackgroundTaskTest, ExecuteJsonCommand)
{
    SharedMemory shared_memory;
    initializeSharedMemory(&shared_memory);
    shared_memory.transmitted_counter++;

    MockComponent     component;
    Parameter<double> parameter(component, "parameter");

    // Simulate a JSON command in shared memory
    StaticJson jsonCommand = {{"name", "MockType.MockName.parameter"}, {"value", 1.5}};
    executeJsonCommand(jsonCommand);
    BufferSwitch::flipState();   // flip the buffer pointer of all settable parameters
    // synchronise new background to new active buffer
    synchroniseReadBuffers();
    EXPECT_EQ(parameter.value(), 1.5);
}

// Tests workflow of receiving JSON commands
TEST_F(BackgroundTaskTest, ReceiveJsonCommand)
{
    SharedMemory shared_memory;
    initializeSharedMemory(&shared_memory);

    MockComponent     component;
    Parameter<double> parameter(component, "parameter");

    // Simulate a JSON command in shared memory
    StaticJson json_command = {{"name", "MockType.MockName.parameter"}, {"value", 1.5}};
    vslib::writeJsonToSharedMemory(json_command, &shared_memory);
    shared_memory.transmitted_counter++;
    received_new_data = false;
    receiveJsonCommand(&shared_memory);
    receiveJsonCommand(&shared_memory);

    EXPECT_EQ(parameter.value(), 1.5);
}
