//! @file
//! @brief File containing method definitions for I/O to the message queues used to communicate JSON commands and
//! parameter map.
//! @author Dominik Arominski

#pragma once

#include "message_queue.hpp"
#include "staticJson.h"

namespace vslib::utils
{
    void writeJsonToMessageQueue(const fgc4::utils::StaticJson&, fgc4::utils::MessageQueueWriter<void>&);
    fgc4::utils::StaticJson readJsonFromMessageQueue(std::span<uint8_t>&);
    void                    writeStringToMessageQueue(const std::string&, fgc4::utils::MessageQueueWriter<void>&);
}   // namespace vslib::utils
