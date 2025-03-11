//! @file
//! @brief File containing method definitions for I/O to the message queues used to communicate JSON commands and
//! parameter map.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "messageQueue.hpp"
#include "staticJson.hpp"

namespace vslib::utils
{
    //! Helper function to serialize JSON object and write it to the message queue.
    //!
    //! @param json_object JSON object to be copied to the shared memory
    //! @param message_queue Reference to the shared memory object
    void writeJsonToMessageQueue(
        const fgc4::utils::StaticJson& json_object, fgc4::utils::MessageQueueWriter<void>& message_queue
    );

    //! Helper function to read JSON object from the message and deserialize it.
    //!
    //! @param message Reference to the shared memory object
    //! @return Static JSON object parsed from shared memory
    const fgc4::utils::StaticJson readJsonFromMessageQueue(std::span<uint8_t>& message);

    //! Helper function to write string object from the message and deserialize it.
    //!
    //! @param message Reference to the string to be written to the queue
    //! @param message_queue Reference to the shared memory object
    void writeStringToMessageQueue(const std::string& message, fgc4::utils::MessageQueueWriter<void>& message_queue);
}   // namespace vslib::utils
