//! @file
//! @brief File containing SharedMemory struct with transaction counters and helper functions used for I/O of JSON
//! objects via shared memory.
//! @author Dominik Arominski

#pragma once

#include <iostream>

#include "nlohmann/json.hpp"

struct SharedMemory
{
    size_t acknowledged_counter{0};
    size_t transmitted_counter{0};
};

// definitions of I/O functions to silence -Wmissing-declarations warnings
void           writeJsonToSharedMemory(const nlohmann::json&, void*, size_t, size_t);
nlohmann::json readJsonFromSharedMemory(void*, size_t);


//! Helper function to serialize JSON object and write to shared memory
//!
//! @param json_object JSON object to be copied to the shared memory
//! @param shared_memory Pointer to the shared memory
//! @param offset Memory offset (in bytes) for the write start point
//! @param shared_memory_size Shared memory size (in bytes)
void writeJsonToSharedMemory(
    const nlohmann::json& json_object, void* shared_memory, size_t offset, size_t shared_memory_size
)
{
    auto serialized = json_object.dump();
    if (serialized.size() + offset <= shared_memory_size)
    {
        std::memcpy(static_cast<char*>(shared_memory) + offset, serialized.data(), serialized.size());
    }
    else
    {
        std::cerr << "Error writing JSON: run out of shared memory.\n";
    }
}

//! Helper function to read JSON object from shared memory and deserialize
//!
//! @param shared_memory Pointer to the shared memory
//! @param offset Memory offset (in bytes) for the write start point
//! @return JSON object parsed from shared memory
nlohmann::json readJsonFromSharedMemory(void* shared_memory, size_t offset)
{
    nlohmann::json json_object;
    auto const     serialized = std::string(static_cast<char*>(shared_memory) + offset);
    try
    {
        json_object = nlohmann::json::parse(serialized);
    }
    catch (const std::exception& e)
    {
        // Handle parsing errors
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
    }
    return json_object;
}
