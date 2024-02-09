//! @file
//! @brief Header file containing background task class responsible for validation of incoming commands,
//! executing them, and triggering synchronisation of buffers.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <bmboot/message_queue.hpp>
#include <nlohmann/json-schema.hpp>

#include "jsonCommandSchema.h"
#include "messageQueue.h"
#include "staticJson.h"

namespace vslib
{
    class ParameterSetting
    {
      public:
        //! Creates the ParameterSetting background task object and initializes the JSON schema validator as well as
        //! read and write JSON queues
        ParameterSetting(uint8_t* read_command_queue_address, uint8_t* write_status_queue_address)
            : m_read_commands_queue{bmboot::createMessageQueue<bmboot::MessageQueueReader<void>>(
                read_command_queue_address, fgc4::utils::constants::json_memory_pool_size
            )},
              m_write_command_status{bmboot::createMessageQueue<bmboot::MessageQueueWriter<void>>(
                  write_status_queue_address, fgc4::utils::constants::string_memory_pool_size
              )}
        {
            m_validator.set_root_schema(utils::json_command_schema);
        }

        void receiveJsonCommand();
        void processJsonCommands(const fgc4::utils::StaticJson&);
        bool validateJsonCommand(const fgc4::utils::StaticJson&);
        void executeJsonCommand(const fgc4::utils::StaticJson&);

        std::optional<fgc4::utils::Warning> validateModifiedComponents();

      private:
        nlohmann::json_schema::json_validator                              m_validator;
        bmboot::MessageQueueReader<void>                                   m_read_commands_queue;
        bmboot::MessageQueueWriter<void>                                   m_write_command_status;
        std::array<uint8_t, fgc4::utils::constants::json_memory_pool_size> m_read_commands_buffer;

        std::array<Component*, 100> m_modified_components{nullptr};
        unsigned short              m_number_modified_components{0};

        void triggerReadBufferSynchronisation();
    };

}   // namespace vslib
