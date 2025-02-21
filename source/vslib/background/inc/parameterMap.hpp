//! @file
//! @brief Header file containing background task class responsible creating and sending
//! the parameter map to the message queue.
//! @author Dominik Arominski

#pragma once

#include "messageQueue.hpp"
#include "rootComponent.hpp"
#include "staticJson.hpp"

namespace vslib
{
    class ParameterMap
    {
      public:
        //! Creates the ParameterMap background task object and initializes the write-direction JSON queue.
        //!
        //! @param address Pointer to the memory address to initialize the write-direction queue
        //! @param queue_size The write-direction queue size in bytes
        //! @param root_component The parent Component to all Components running in the binary
        ParameterMap(uint8_t* address, const size_t queue_size, RootComponent& root_component)
            : m_write_parameter_map_queue{fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueWriter<void>>(
                address, queue_size
            )},
              m_root_component(root_component)
        {
        }

        //! Creates and uploads the parameter map to the shared memory. The static heap of the StaticJson is
        //! reinitialized each time this method is called.
        void uploadParameterMap();

      private:
        fgc4::utils::MessageQueueWriter<void> m_write_parameter_map_queue;   //!< Write-direction queue
        RootComponent&                        m_root_component;              //!< Root Component to the running binary
    };

}   // namespace vslib
