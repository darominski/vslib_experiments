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
    SharedMemory   shared_memory;
    BackgroundTask backgroundTask(shared_memory);
    EXPECT_EQ(shared_memory.status, CommunicationStatus::ready_to_receive);
    EXPECT_EQ(shared_memory.message_length, 0);
    for (auto const& element : shared_memory.json_buffer)
    {
        EXPECT_EQ(element, std::byte());
    }
}

// Tests triggering creation of parameter map and its uploading to the shared memory structure
TEST_F(BackgroundTaskTest, UploadParameterMap)
{
    SharedMemory       shared_memory;
    BackgroundTask     backgroundTask(shared_memory);
    MockComponent      component;
    Parameter<int32_t> parameter(component, "parameter");
    backgroundTask.uploadParameterMap();
    EXPECT_EQ(shared_memory.message_length, 146);
    nlohmann::json parameter_map = nlohmann::json::parse(
        shared_memory.json_buffer.begin(), shared_memory.json_buffer.begin() + shared_memory.message_length
    );
    EXPECT_NE(parameter_map, nlohmann::json::object());
    EXPECT_TRUE(parameter_map.is_array());
    EXPECT_TRUE(parameter_map[0].is_object());
    EXPECT_TRUE(parameter_map[0].contains("version"));
    EXPECT_EQ(parameter_map[0]["version"], "0.1");
    EXPECT_TRUE(parameter_map[1].is_object());
    EXPECT_TRUE(parameter_map[1].contains("components"));
    EXPECT_TRUE(parameter_map[1].contains("parameters"));
    EXPECT_TRUE(parameter_map[1].contains("name"));
    EXPECT_TRUE(parameter_map[1].contains("type"));
    EXPECT_TRUE(parameter_map[1]["parameters"][0].contains("length"));
    EXPECT_TRUE(parameter_map[1]["parameters"][0].contains("name"));
    EXPECT_TRUE(parameter_map[1]["parameters"][0].contains("type"));
    EXPECT_TRUE(parameter_map[1]["parameters"][0].contains("value"));
    EXPECT_EQ(parameter_map[1]["components"], nlohmann::json::array());
    EXPECT_EQ(parameter_map[1]["name"], "MockName");
    EXPECT_EQ(parameter_map[1]["type"], "MockType");
    EXPECT_EQ(parameter_map[1]["parameters"][0]["length"], 1);
    EXPECT_EQ(parameter_map[1]["parameters"][0]["value"], nlohmann::json::object());
    EXPECT_EQ(parameter_map[1]["parameters"][0]["name"], "parameter");
    EXPECT_EQ(parameter_map[1]["parameters"][0]["type"], "Int32");
}

// Tests validation of the incoming json command
TEST_F(BackgroundTaskTest, ValidateJsonCommand)
{
    SharedMemory   shared_memory;
    BackgroundTask backgroundTask(shared_memory);

    StaticJson command_no_name  = {{"value", 1.0}};
    StaticJson command_no_value = {{"name", "p"}};
    StaticJson command_valid    = {{"name", "p"}, {"value", 1}, {"version", "0.1"}};

    EXPECT_EQ(backgroundTask.validateJsonCommand(command_no_name), false);
    EXPECT_EQ(backgroundTask.validateJsonCommand(command_no_value), false);
    EXPECT_EQ(backgroundTask.validateJsonCommand(command_valid), true);
}

// Tests execution workflow of JSON command
TEST_F(BackgroundTaskTest, ExecuteJsonCommand)
{
    SharedMemory   shared_memory;
    BackgroundTask backgroundTask(shared_memory);

    MockComponent     component;
    Parameter<double> parameter(component, "parameter");

    // Simulate a JSON command in shared memory
    StaticJson jsonCommand = {{"name", "MockType.MockName.parameter"}, {"value", 1.5}, {"version", "0.1"}};
    backgroundTask.executeJsonCommand(jsonCommand);
    BufferSwitch::flipState();   // flip the buffer pointer of all settable parameters
    EXPECT_EQ(parameter.value(), 1.5);
}

// Tests workflow of receiving JSON commands
TEST_F(BackgroundTaskTest, ReceiveJsonCommand)
{
    SharedMemory   shared_memory;
    BackgroundTask backgroundTask(shared_memory);

    MockComponent     component;
    Parameter<double> parameter(component, "parameter");

    // Simulate a JSON command in shared memory
    StaticJson json_command = {{"name", "MockType.MockName.parameter"}, {"value", 1.5}, {"version", "0.1"}};
    writeJsonToSharedMemory(json_command, shared_memory);

    backgroundTask.receiveJsonCommand();
    // second call to acknowledge that no new data is coming and thus trigger buffer switch flip
    backgroundTask.receiveJsonCommand();

    EXPECT_EQ(parameter.value(), 1.5);
}
