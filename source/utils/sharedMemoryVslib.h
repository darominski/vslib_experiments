//! @file
//! @brief File containing SharedMemory struct with transaction counters and helper functions used for I/O of JSON
//! objects via shared memory.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <iostream>

#include "constants.h"
#include "errorMessage.h"
#include "fmt/format.h"
#include "json/json.hpp"

namespace vslib
{
    struct SharedMemory
    {
        std::size_t                                                          acknowledged_counter{0};
        std::size_t                                                          transmitted_counter{0};
        std::size_t                                                          message_length{0};
        std::array<std::byte, fgc4::utils::constants::json_memory_pool_size> json_buffer;
    };

#define SHARED_MEMORY_ADDRESS 0x802000000
#define SHARED_MEMORY_SIZE    fgc4::utils::constants::json_memory_pool_size

#define SHARED_MEMORY (*(struct SharedMemory* volatile)SHARED_MEMORY_ADDRESS)

    // ************************************************************

    // definitions of I/O functions to silence -Wmissing-declarations warnings
    void           writeJsonToSharedMemory(const nlohmann::json&, SharedMemory*);
    nlohmann::json readJsonFromSharedMemory(SharedMemory*);

    //! Helper function to serialize JSON object and write to shared memory
    //!
    //! @param json_object JSON object to be copied to the shared memory
    //! @param shared_memory Reference to the shared memory object
    void writeJsonToSharedMemory(const nlohmann::json& json_object, SharedMemory* shared_memory)
    {
        auto serialized = json_object.dump();
        if (serialized.size() < fgc4::utils::constants::json_memory_pool_size)
        {
            std::memcpy(
                reinterpret_cast<char*>(shared_memory->json_buffer.begin()), serialized.data(), serialized.size()
            );
        }
        else
        {
            std::cerr << fmt::format(
                "{}",
                fgc4::utils::Error(
                    "Error writing JSON: run out of shared memory.\n",
                    fgc4::utils::constants::error_allocation_buffer_overflow
                )
            );
            throw std::bad_alloc();
        }
        shared_memory->message_length = serialized.size();
    }

    //! Helper function to read JSON object from shared memory and deserialize it
    //!
    //! @param shared_memory Reference to the shared memory object
    //! @return JSON object parsed from shared memory
    nlohmann::json readJsonFromSharedMemory(SharedMemory* shared_memory)
    {
        nlohmann::json json_object;
        try
        {
            json_object = nlohmann::json::parse(
                shared_memory->json_buffer.begin(), shared_memory->json_buffer.begin() + shared_memory->message_length
            );
        }
        catch (const std::exception& e)
        {
            // Handle parsing errors
            fgc4::utils::Error error_msg(
                std::string("Error parsing JSON: ") + e.what() + std::string("\n"),
                fgc4::utils::constants::error_json_command_invalid
            );
            std::cerr << fmt::format("{}", error_msg);
        }
        return json_object;
    }
}   // namespace vslib
