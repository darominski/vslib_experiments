#pragma once

#include <iostream>

#include "nlohmann/json.hpp"

struct SharedMemory
{
    size_t acknowledged_counter{0};
    size_t transmitted_counter{0};
};

void           writeJsonToSharedMemory(const nlohmann::json&, void*, size_t, size_t);
nlohmann::json readJsonFromSharedMemory(void*, size_t);


// Helper function to serialize JSON object and write to shared memory
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

// Helper function to read JSON object from shared memory and deserialize
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
