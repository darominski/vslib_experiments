//! @file
//! @brief Header file containing background task class responsible creating and sending
//! the parameter map to the message queue.
//! @author Dominik Arominski

#pragma once

#include <bmboot/message_queue.hpp>

#include "sharedMemory.h"
#include "staticJson.h"
#include "vslib_shared_memory_memmap.h"

namespace vslib
{
    class ParameterMap
    {
      public:
        //! Creates the ParameterMap background task object and initializes the write JSON queue
        ParameterMap()
            : m_write_parameter_map_queue{bmboot::createMessageQueue<bmboot::MessageQueueWriter<void>>(
                (uint8_t*)app_data_0_1_ADDRESS
                    + fgc4::utils::constants::json_memory_pool_size,   // offset with regard to read queue, TO-DO: have
                                                                       // it as part of memory mapping
                fgc4::utils::constants::json_memory_pool_size
            )}
        {
        }

        void uploadParameterMap();

      private:
        bmboot::MessageQueueWriter<void> m_write_parameter_map_queue;
    };

}   // namespace vslib