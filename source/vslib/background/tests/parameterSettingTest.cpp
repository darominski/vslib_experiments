//! @file
//! @brief File with unit tests of ParameterSetting background-task class.
//! @author Dominik Arominski

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "component.h"
#include "json/json.hpp"
#include "messageQueue.h"
#include "parameter.h"
#include "parameterSetting.h"
#include "staticJson.h"

using namespace vslib;

class ParameterSettingTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ComponentRegistry& component_registry = ComponentRegistry::instance();
        component_registry.clearRegistry();
        ParameterRegistry& parameter_registry = ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    void TearDown() override
    {
    }
};

template<typename T>
class MockComponent : public Component
{
  public:
    MockComponent(std::string_view type, std::string_view name, Component* parent = nullptr)
        : Component(type, name, parent),
          parameter(*this, "parameter")
    {
    }

    Parameter<T> parameter;
};

//! Checks that a ParameterSetting object can be constructed
TEST_F(ParameterSettingTest, ParameterSettingDefaultConstruction)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ASSERT_NO_THROW(ParameterSetting(read_buffer.data(), write_buffer.data()));
}

//! Checks that a ParameterSetting correct commadn is properly validated
TEST_F(ParameterSettingTest, ParameterSettingValidateCorrectCommand)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    nlohmann::json test_command = {{"name", "test"}, {"value", 1.0}, {"version", std::array<int, 2>{0, 1}}};
    ASSERT_TRUE(parameter_setting.validateJsonCommand(test_command));
}

//! Checks that a ParameterSetting command validation finds out that the command is missing fields
TEST_F(ParameterSettingTest, ParameterSettingValidateIncorrectCommand)
{
    constexpr size_t                queue_size = 1024;   // 1024 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    auto read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json missing_name = {{"value", 1.0}, {"version", std::array<int, 2>{0, 1}}};
    EXPECT_FALSE(parameter_setting.validateJsonCommand(missing_name));

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            message = read_queue.read(read_message_buffer);
    EXPECT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "Command invalid: At  of {\"value\":1.0,\"version\":[0,1]} - required property 'name' not found in object\n"
    );

    nlohmann::json missing_value = {{"name", "test"}, {"version", std::array<int, 2>{0, 1}}};
    EXPECT_FALSE(parameter_setting.validateJsonCommand(missing_value));
    message = read_queue.read(read_message_buffer);
    EXPECT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "Command invalid: At  of {\"name\":\"test\",\"version\":[0,1]} - required property 'value' not found in "
        "object\n"
    );

    nlohmann::json missing_version = {{"name", "test"}, {"value", 1.0}};
    EXPECT_FALSE(parameter_setting.validateJsonCommand(missing_version));
    message = read_queue.read(read_message_buffer);
    EXPECT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "Command invalid: At  of {\"name\":\"test\",\"value\":1.0} - required property 'version' not found in object\n"
    );

    nlohmann::json wrong_type_version
        = {{"name", "test"}, {"value", 1.0}, {"version", "0,1"}};   // version ought to be an array
    EXPECT_FALSE(parameter_setting.validateJsonCommand(wrong_type_version));
    message = read_queue.read(read_message_buffer);
    EXPECT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "Command invalid: At /version of \"0,1\" - unexpected instance type\n"
    );

    nlohmann::json unsupported_version = {{"name", "test"}, {"value", 1.0}, {"version", std::array<int, 2>{99, 0}}};
    EXPECT_FALSE(parameter_setting.validateJsonCommand(unsupported_version));
    message = read_queue.read(read_message_buffer);
    EXPECT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "Inconsistent major version of the communication interface! Provided version: 99, expected version: 0.\n"
    );
}

//! Checks that a ParameterSetting can process a single int16 command
TEST_F(ParameterSettingTest, ParameterSettingProcessSingleIntCommand)
{
    constexpr size_t                queue_size = 1e4;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int16_t> test(type, name);

    nlohmann::json single_command
        = {{"name", type + '.' + name + ".parameter"}, {"value", 1}, {"version", std::array<int, 2>{0, 1}}};
    ASSERT_NO_THROW(parameter_setting.processJsonCommands(single_command));
    test.flipBufferState();
    EXPECT_EQ(test.parameter, (int16_t)single_command["value"]);
}

//! Checks that a ParameterSetting can process a single uint32 command
TEST_F(ParameterSettingTest, ParameterSettingProcessSingleUintCommand)
{
    constexpr size_t                queue_size = 1e4;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    std::string             type = "type";
    std::string             name = "name";
    MockComponent<uint32_t> test(type, name);

    nlohmann::json single_command
        = {{"name", type + '.' + name + ".parameter"}, {"value", (uint32_t)5}, {"version", std::array<int, 2>{0, 1}}};
    ASSERT_NO_THROW(parameter_setting.processJsonCommands(single_command));
    test.flipBufferState();
    EXPECT_EQ(test.parameter, (uint32_t)single_command["value"]);
}

//! Checks that a ParameterSetting can process a single command
TEST_F(ParameterSettingTest, ParameterSettingProcessSingleDoubleCommand)
{
    constexpr size_t                queue_size = 1e4;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    std::string           type = "type";
    std::string           name = "name";
    MockComponent<double> test(type, name);

    nlohmann::json single_command
        = {{"name", type + '.' + name + ".parameter"}, {"value", 3.14159}, {"version", std::array<int, 2>{0, 1}}};
    ASSERT_NO_THROW(parameter_setting.processJsonCommands(single_command));
    test.flipBufferState();
    EXPECT_EQ(test.parameter, (double)single_command["value"]);
}

//! Checks that a ParameterSetting finds that the type of the provided command value does not match uint32
TEST_F(ParameterSettingTest, ParameterSettingProcessSingleIncorrectUintCommand)
{
    constexpr size_t                queue_size = 1e4;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    std::string             type = "type";
    std::string             name = "name";
    MockComponent<uint32_t> test(type, name);

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command
        = {{"name", type + '.' + name + ".parameter"}, {"value", -5}, {"version", std::array<int, 2>{0, 1}}};
    parameter_setting.processJsonCommands(single_command);

    auto message = read_queue.read(read_message_buffer);
    ASSERT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "The provided command value: -5 is not an unsigned integer, while Parameter type is an unsigned integer.\n"
    );

    // test.flipBufferState();
    EXPECT_NE(test.parameter, (uint32_t)single_command["value"]);
}

//! Checks that a ParameterSetting finds that the type of the provided command value does not match int64
TEST_F(ParameterSettingTest, ParameterSettingProcessSingleIncorrectIntCommand)
{
    constexpr size_t                queue_size = 1e4;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int64_t> test(type, name);

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command
        = {{"name", type + '.' + name + ".parameter"}, {"value", 3.14159}, {"version", std::array<int, 2>{0, 1}}};
    parameter_setting.processJsonCommands(single_command);

    auto message = read_queue.read(read_message_buffer);
    ASSERT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "The provided command value: 3.14159 is not an integer, while Parameter type is an integer.\n"
    );
    EXPECT_NE(test.parameter, (int64_t)single_command["value"]);
}

//! Checks that a ParameterSetting sets correctly a number of commands, with the last one being used in this case
TEST_F(ParameterSettingTest, ParameterMapProcessArrayCommand)
{
    constexpr size_t                queue_size = 1e4;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int32_t> test(type, name);

    nlohmann::json single_command
        = {{"name", type + '.' + name + ".parameter"}, {"value", 1}, {"version", std::array<int, 2>{0, 1}}};
    nlohmann::json multiple_commands = {single_command, single_command, single_command};
    int32_t        new_value         = 5;
    multiple_commands[2]["value"]    = new_value;
    ASSERT_NO_THROW(parameter_setting.processJsonCommands(multiple_commands));
    test.flipBufferState();
    EXPECT_EQ(test.parameter, new_value);
}

//! Checks that a ParameterSetting sets correctly a number of commands, with the last one being used in this case
TEST_F(ParameterSettingTest, ParameterMapProcessArrayInvalidCommand)
{
    constexpr size_t                queue_size = 1e4;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data());

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int32_t> test(type, name);

    nlohmann::json single_command
        = {{"name", type + '.' + name + ".parameter"}, {"value", 1}, {"version", std::array<int, 2>{0, 1}}};
    nlohmann::json multiple_commands  = {single_command, single_command, single_command};
    int32_t        new_accepted_value = 2;
    int32_t        new_refused_value  = 5;
    multiple_commands[1]["value"]     = new_accepted_value;
    multiple_commands[2]["value"]     = new_refused_value;
    // break the last command:
    multiple_commands[2]["version"]   = "";
    ASSERT_NO_THROW(parameter_setting.processJsonCommands(multiple_commands));
    test.flipBufferState();
    EXPECT_EQ(test.parameter, new_accepted_value);
}

// //! Checks that a ParameterSetting executes json commands
// TEST_F(ParameterSettingTest, ParameterMapExecuteJsonCommand)
// {
//     constexpr size_t                queue_size = 100;   // 100 bytes
//     std::array<uint8_t, queue_size> read_buffer{};
//     std::array<uint8_t, queue_size> write_buffer{};
//     ASSERT_NO_THROW(ParameterSetting(read_buffer.data(), write_buffer.data()));
// }

// //! Checks that a ParameterSetting validates modified components correctly
// TEST_F(ParameterSettingTest, ParameterMapValidateModifiedComponents)
// {
//     constexpr size_t                queue_size = 100;   // 100 bytes
//     std::array<uint8_t, queue_size> read_buffer{};
//     std::array<uint8_t, queue_size> write_buffer{};
//     ASSERT_NO_THROW(ParameterSetting(read_buffer.data(), write_buffer.data()));
// }
