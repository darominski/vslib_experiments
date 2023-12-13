//! @file
//! @brief File containing SharedMemory struct with transaction counters and helper functions used for I/O of JSON
//! objects via shared memory.
//! @author Dominik Arominski

#pragma once

#include <array>

#include "constants.h"
#include "errorCodes.h"
#include "errorMessage.h"
#include "fmt/format.h"
#include "staticJson.h"
#include "vslib_shared_memory_memmap.h"

namespace vslib
{
    struct SharedMemory
    {
        std::size_t                                                          acknowledged_counter;
        std::size_t                                                          transmitted_counter;
        std::size_t                                                          message_length;
        std::array<std::byte, fgc4::utils::constants::json_memory_pool_size> json_buffer;
    };

    static_assert(sizeof(SharedMemory) <= app_data_0_1_SIZE);
    static_assert(sizeof(SharedMemory) <= app_data_0_2_SIZE);
    static_assert(sizeof(SharedMemory) <= app_data_0_3_SIZE);

    // ************************************************************

    // definitions of I/O functions to silence -Wmissing-declarations warnings
    void                    initializeSharedMemory(SharedMemory&);
    void                    writeJsonToSharedMemory(const fgc4::utils::StaticJson&, SharedMemory&);
    fgc4::utils::StaticJson readJsonFromSharedMemory(SharedMemory&);

}   // namespace vslib
