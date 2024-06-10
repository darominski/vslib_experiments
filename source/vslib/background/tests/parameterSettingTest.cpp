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
#include "typeTraits.h"

using namespace vslib;

class ParameterSettingTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
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
    MockComponent(std::string_view type, std::string_view name, Component* parent)
        : Component(type, name, parent),
          parameter(*this, "parameter")
    {
    }

    Parameter<T> parameter;

    //! Validation workflow, for integral types checks if the value is even, pass-through otherwise
    std::optional<fgc4::utils::Warning> verifyParameters() override
    {
        if constexpr (fgc4::utils::Integral<T>)
        {
            if (parameter.toValidate() % 2 != 0)
            {
                return fgc4::utils::Warning("Parameter value must be even\n");
            }
        }
        return {};
    }
};

//! Checks that a ParameterSetting object can be constructed
TEST_F(ParameterSettingTest, ParameterSettingDefaultConstruction)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("test_type", "test_name", nullptr);

    ASSERT_NO_THROW(ParameterSetting(read_buffer.data(), write_buffer.data(), root_component));
}

//! Checks that a ParameterSetting correct commadn is properly validated
TEST_F(ParameterSettingTest, ParameterSettingValidateCorrectCommand)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("test_type", "test_name", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    nlohmann::json test_command = {{"name", "test"}, {"value", 1.0}, {"version", std::array<int, 2>{0, 1}}};
    ASSERT_TRUE(parameter_setting.validateJsonCommand(test_command));
}

//! Checks that a ParameterSetting command validation finds out that the command is missing fields
TEST_F(ParameterSettingTest, ParameterSettingValidateIncorrectCommand)
{
    constexpr size_t                queue_size = 1024;   // 1024 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("test_type", "test_name", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

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
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int16_t> test(type, name, &root_component);

    nlohmann::json single_command = {
        {"name", "root.root." + type + '.' + name + ".parameter"}, {"value", 1}, {"version", std::array<int, 2>{0, 1}}};
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
    Component                       root_component("root", "root", nullptr);
    ParameterSetting                parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string             type = "type";
    std::string             name = "name";
    MockComponent<uint32_t> test(type, name, &root_component);

    nlohmann::json single_command
        = {{"name", root_component.getFullName() + '.' + type + '.' + name + ".parameter"},
           {"value", (uint32_t)5},
           {"version", std::array<int, 2>{0, 1}}};
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
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string           type = "type";
    std::string           name = "name";
    MockComponent<double> test(type, name, &root_component);

    nlohmann::json single_command
        = {{"name", root_component.getFullName() + '.' + type + '.' + name + ".parameter"},
           {"value", 3.14159},
           {"version", std::array<int, 2>{0, 1}}};
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
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string             type = "type";
    std::string             name = "name";
    MockComponent<uint32_t> test(type, name, &root_component);

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command
        = {{"name", root_component.getFullName() + '.' + type + '.' + name + ".parameter"},
           {"value", -5},
           {"version", std::array<int, 2>{0, 1}}};
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
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int64_t> test(type, name, &root_component);

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command
        = {{"name", root_component.getFullName() + '.' + type + '.' + name + ".parameter"},
           {"value", 3.14159},
           {"version", std::array<int, 2>{0, 1}}};
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
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int32_t> test(type, name, &root_component);

    nlohmann::json single_command
        = {{"name", root_component.getFullName() + '.' + type + '.' + name + ".parameter"},
           {"value", 1},
           {"version", std::array<int, 2>{0, 1}}};
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
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int32_t> test(type, name, &root_component);

    nlohmann::json single_command
        = {{"name", root_component.getFullName() + '.' + type + '.' + name + ".parameter"},
           {"value", 1},
           {"version", std::array<int, 2>{0, 1}}};
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

//! Checks that a ParameterSetting executes a json command correctly
TEST_F(ParameterSettingTest, ParameterMapExecuteCorrectCommand)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int32_t> test(type, name, &root_component);
    int32_t                value = 1;

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command
        = {{"name", root_component.getFullName() + '.' + type + '.' + name + ".parameter"},
           {"value", value},
           {"version", std::array<int, 2>{0, 1}}};

    ASSERT_NO_THROW(parameter_setting.executeJsonCommand(single_command));

    auto message = read_queue.read(read_message_buffer);
    ASSERT_TRUE(message.has_value());
    EXPECT_EQ(std::string(message.value().begin(), message.value().end()), "Parameter value updated successfully.\n");

    test.flipBufferState();
    EXPECT_EQ(test.parameter, value);
    EXPECT_TRUE(test.parametersInitialized());
}

//! Checks that a ParameterSetting catches an invalid json command correctly
TEST_F(ParameterSettingTest, ParameterMapExecuteIncorrectCommand)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type = "type";
    std::string            name = "name";
    MockComponent<int32_t> test(type, name, &root_component);
    int32_t                value = 1;

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command = {{"name", "invalid"}, {"value", value}, {"version", std::array<int, 2>{0, 1}}};

    ASSERT_NO_THROW(parameter_setting.executeJsonCommand(single_command));

    auto message = read_queue.read(read_message_buffer);
    ASSERT_TRUE(message.has_value());
    EXPECT_EQ(
        std::string(message.value().begin(), message.value().end()),
        "Parameter ID: invalid not found. Command ignored.\n"
    );

    test.flipBufferState();
    EXPECT_NE(test.parameter, value);
    EXPECT_FALSE(test.parametersInitialized());
}

//! Checks that a ParameterSetting validates modified components correctly
TEST_F(ParameterSettingTest, ParameterMapValidateCorrectModifiedComponents)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type  = "type";
    std::string            name1 = "name1";
    std::string            name2 = "name2";
    MockComponent<int32_t> component_1(type, name1, &root_component);
    MockComponent<int32_t> component_2(type, name2, &root_component);
    int32_t                value = 2;

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command = value;
    component_1.parameter.setJsonValue(single_command);

    EXPECT_TRUE(component_1.parametersInitialized());
    EXPECT_FALSE(component_2.parametersInitialized());   // unrelated component should not be flagged as modified

    parameter_setting.validateComponents();

    EXPECT_EQ(component_1.parameter, value);
    EXPECT_NE(component_2.parameter, value);
    EXPECT_TRUE(component_1.parametersInitialized());
    EXPECT_FALSE(component_2.parametersInitialized());
}

//! Checks that a ParameterSetting validates modified hierarchical components correctly
TEST_F(ParameterSettingTest, ParameterMapValidateCorrectModifiedHierarchicalComponents)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type  = "type";
    std::string            name1 = "name1";
    std::string            name2 = "name2";
    MockComponent<int32_t> component_1(type, name1, &root_component);
    MockComponent<int32_t> component_2(type, name2, &component_1);
    int32_t                value = 2;

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command = value;
    component_2.parameter.setJsonValue(single_command);

    // both comoponents should be flagged as modified along the hierarchy
    EXPECT_FALSE(component_1.parametersInitialized());
    EXPECT_TRUE(component_2.parametersInitialized());

    parameter_setting.validateComponents();

    EXPECT_NE(component_1.parameter, value);   // not expected to be modified
    EXPECT_EQ(component_2.parameter, value);

    // flags should not be changed by running the validation of components
    EXPECT_FALSE(component_1.parametersInitialized());
    EXPECT_TRUE(component_2.parametersInitialized());
}

//! Checks that a ParameterSetting validates modified components correctly
TEST_F(ParameterSettingTest, ParameterMapValidateIncorrectModifiedComponents)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type  = "type";
    std::string            name1 = "name1";
    std::string            name2 = "name2";
    MockComponent<int32_t> component_1(type, name1, &root_component);
    MockComponent<int32_t> component_2(type, name2, &root_component);
    int32_t                value = 3;

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command = value;
    component_1.parameter.setJsonValue(single_command);

    EXPECT_TRUE(component_1.parametersInitialized());
    EXPECT_FALSE(component_2.parametersInitialized());   // unrelated component should not be flagged as modified

    parameter_setting.validateComponents();

    EXPECT_NE(component_1.parameter, value);
    EXPECT_NE(component_2.parameter, value);
    EXPECT_TRUE(component_1.parametersInitialized());
    EXPECT_FALSE(component_2.parametersInitialized());

    // ensures that despite incorrect input, the value has not been modified after flipping the buffer:
    component_1.flipBufferState();
    EXPECT_NE(component_1.parameter, value);
}

//! Checks that a ParameterSetting validates modified hierarchical components correctly
TEST_F(ParameterSettingTest, ParameterMapValidateIncorrectModifiedHierarchicalComponents)
{
    constexpr size_t                queue_size = 100;   // 100 bytes
    std::array<uint8_t, queue_size> read_buffer{};
    std::array<uint8_t, queue_size> write_buffer{};
    Component                       root_component("root", "root", nullptr);

    ParameterSetting parameter_setting(read_buffer.data(), write_buffer.data(), root_component);

    std::string            type  = "type";
    std::string            name1 = "name1";
    std::string            name2 = "name2";
    MockComponent<int32_t> component_1(type, name1, &root_component);
    MockComponent<int32_t> component_2(type, name2, &component_1);
    int32_t                value = 3;

    std::array<uint8_t, queue_size> read_message_buffer;
    auto                            read_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)write_buffer.data(), queue_size
    );

    nlohmann::json single_command = value;
    component_2.parameter.setJsonValue(single_command);

    EXPECT_FALSE(component_1.parametersInitialized());
    EXPECT_TRUE(component_2.parametersInitialized());

    parameter_setting.validateComponents();

    EXPECT_FALSE(component_1.parametersInitialized());
    EXPECT_TRUE(component_2.parametersInitialized());

    EXPECT_NE(component_1.parameter, value);
    EXPECT_NE(component_2.parameter, value);

    // ensures that despite incorrect input, the value has not been modified after flipping the buffer:
    component_1.flipBufferState();
    EXPECT_NE(component_1.parameter, value);
    EXPECT_NE(component_2.parameter, value);
}
