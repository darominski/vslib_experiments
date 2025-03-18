//! @file
//! @brief Header file containing background task class responsible for validation of incoming commands,
//! executing them, and triggering synchronisation of buffers.
//! @author Dominik Arominski

#pragma once

#include <nlohmann/json-schema.hpp>

#include "component.hpp"
#include "jsonCommandSchema.hpp"
#include "messageQueue.hpp"
#include "rootComponent.hpp"
#include "staticJson.hpp"
#include "vslibMessageQueue.hpp"

namespace vslib
{
    class ParameterSetting
    {
      public:
        //! Creates the ParameterSetting background task object and initializes the JSON schema validator as well as
        //! read and write JSON queues
        ParameterSetting(
            uint8_t* read_command_queue_address, uint8_t* write_status_queue_address, RootComponent& root_component
        )
            : m_read_commands_queue{fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
                read_command_queue_address, fgc4::utils::constants::json_memory_pool_size
            )},
              m_write_command_status{fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueWriter<void>>(
                  write_status_queue_address, fgc4::utils::constants::string_memory_pool_size
              )},
              m_root_component(root_component)
        {
            m_validator.set_root_schema(utils::json_command_schema);
        }

        //! Checks if a new command has arrived in shared memory, processes it, and when
        //! new command has come previously switches buffers and calls to synchronise them
        void receiveJsonCommand();

        //! Processes the received JSON commands, checking whether one or many commands were received.
        //!
        //! @param command JSON object containing one or more JSON commands to be executed
        void processJsonCommands(const fgc4::utils::StaticJson& command);

        //! Validates the provided json command.
        //!
        //! @param command JSON object to be validated as a valid command
        //! @return True if the command contains all expected fields, false otherwise.
        bool validateJsonCommand(const fgc4::utils::StaticJson& command);

        //! Executes a single JSON command by setting the received command value to the parameter reference
        //! stored in ParameterRegistry identified by the command's parameter name.
        //!
        //! @param command JSON object containing name of the parameter to be modified, and the new value with its type
        //! to be inserted
        void executeJsonCommand(const fgc4::utils::StaticJson& command);

        //! Calls verifyParameters of all Components with initialized Parameters attached to the root Component.
        //! Any raised warnings are forwarded to the output status queue.
        void validateComponents();

        //! Checks if there are any new commands available in the queue.
        //!
        //! @return True if there are new objects in the read queue, false otherwise.
        bool checkNewSettingsAvailable();

      private:
        nlohmann::json_schema::json_validator m_validator{};            //!< JSON schema for incoming commands
        fgc4::utils::MessageQueueReader<void> m_read_commands_queue;    //!< Incoming commands queue
        fgc4::utils::MessageQueueWriter<void> m_write_command_status;   //!< Command execution status queue

        //!< Buffer for the incoming commands
        std::array<uint8_t, fgc4::utils::constants::json_memory_pool_size> m_read_commands_buffer{0};

        RootComponent& m_root_component;   //!< Root Component

        //! Recursive function to call verifyParameters on the component and its children
        void validateComponent(const ChildrenList&);
    };

}   // namespace vslib
