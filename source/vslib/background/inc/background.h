//! @file
//! @brief Source file containing library-side background task specific declarations for creating and uploading the
//! parameter map, validation of incoming commands, executing them, and triggering synchronisation of buffers.
//! @author Dominik Arominski

#pragma once

#include <bmboot/message_queue.hpp>
#include <nlohmann/json-schema.hpp>

#include "jsonCommandSchema.h"
#include "sharedMemory.h"
#include "staticJson.h"

namespace vslib
{
    class BackgroundTask
    {
      public:
        //! Creates the background task object and initializes the JSON schema validator as well as read and write JSON
        //! queues
        BackgroundTask()
            : m_read_commands_queue{bmboot::createMessageQueue<bmboot::MessageQueueReader<void>>(
                (uint8_t*)app_data_0_1_ADDRESS, fgc4::utils::constants::json_memory_pool_size
            )},
              m_write_parameter_map_queue{bmboot::createMessageQueue<bmboot::MessageQueueWriter<void>>(
                  (uint8_t*)app_data_0_1_ADDRESS + fgc4::utils::constants::json_memory_pool_size,
                  fgc4::utils::constants::json_memory_pool_size
              )}
        {
            m_validator.set_root_schema(utils::json_command_schema);
        }

        void uploadParameterMap();

        void receiveJsonCommand();
        void processJsonCommands(const fgc4::utils::StaticJson&);
        bool validateJsonCommand(const fgc4::utils::StaticJson&);
        void executeJsonCommand(const fgc4::utils::StaticJson&);

      private:
        bool                                                               m_received_new_data{false};
        nlohmann::json_schema::json_validator                              m_validator;
        bmboot::MessageQueueReader<void>                                   m_read_commands_queue;
        bmboot::MessageQueueWriter<void>                                   m_write_parameter_map_queue;
        std::array<uint8_t, fgc4::utils::constants::json_memory_pool_size> m_read_commands_buffer;

        void triggerReadBufferSynchronisation();
    };

}   // namespace vslib::backgroundTask
