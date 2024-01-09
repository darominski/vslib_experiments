//! @file
//! @brief File containing SharedMemory struct with transaction counters and helper functions used for I/O of JSON
//! objects via shared memory.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <bmboot/message_queue.hpp>

#include "constants.h"
#include "errorCodes.h"
#include "errorMessage.h"
#include "fmt/format.h"
#include "staticJson.h"
#include "vslib_shared_memory_memmap.h"

namespace vslib
{
    void                    writeJsonToMessageQueue(const fgc4::utils::StaticJson&, bmboot::MessageQueueWriter<void>&);
    fgc4::utils::StaticJson readJsonFromMessageQueue(std::span<uint8_t>&);
}   // namespace vslib
