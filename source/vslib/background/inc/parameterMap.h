//! @file
//! @brief Header file containing background task class responsible creating and sending
//! the parameter map to the message queue.
//! @author Dominik Arominski

#pragma once

#include "component.h"
#include "messageQueue.h"
#include "staticJson.h"

namespace vslib
{
    class ParameterMap
    {
      public:
        //! Creates the ParameterMap background task object and initializes the write JSON queue
        ParameterMap(uint8_t* address, size_t queue_size, Component& root_component)
            : m_write_parameter_map_queue{fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueWriter<void>>(
                address, queue_size
            )},
              m_root_component(root_component)
        {
        }

        void uploadParameterMap();

      private:
        fgc4::utils::MessageQueueWriter<void> m_write_parameter_map_queue;
        Component&                            m_root_component;
    };

}   // namespace vslib
