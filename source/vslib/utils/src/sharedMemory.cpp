//! @file
//! @brief File containing implementation of methods used to manipulate the shared memory object.
//! @author Dominik Arominski

#include "sharedMemory.h"
#include "warningMessage.h"

namespace vslib
{

    //! Memory that initializes shared memory structure fields to a known state
    //!
    //! @param shared_memory Shared memory object
    void initializeSharedMemory(SharedMemory& shared_memory)
    {
        shared_memory.acknowledged_counter = 0;
        shared_memory.transmitted_counter  = 0;
        shared_memory.message_length       = 0;
        for (auto& element : shared_memory.json_buffer)
        {
            element = std::byte();
        }
    }

    //! Helper function to serialize JSON object and write to shared memory
    //!
    //! @param json_object JSON object to be copied to the shared memory
    //! @param shared_memory Reference to the shared memory object
    void writeJsonToSharedMemory(const fgc4::utils::StaticJson& json_object, SharedMemory& shared_memory)
    {
        auto serialized = json_object.dump();
        if (serialized.size() < fgc4::utils::constants::json_memory_pool_size)
        {
            std::memcpy(
                reinterpret_cast<char*>(shared_memory.json_buffer.begin()), serialized.data(), serialized.size()
            );
        }
        else
        {
            fgc4::utils::Error(
                "Error writing JSON: run out of shared memory.\n", fgc4::utils::errorCodes::allocation_buffer_overflow
            );
            throw std::bad_alloc();
        }
        shared_memory.message_length = serialized.size();
    }

    //! Helper function to read JSON object from shared memory and deserialize it
    //!
    //! @param shared_memory Reference to the shared memory object
    //! @return Static JSON object parsed from shared memory
    fgc4::utils::StaticJson readJsonFromSharedMemory(SharedMemory& shared_memory)
    {
        auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
        try
        {
            json_object = nlohmann::json::parse(
                shared_memory.json_buffer.begin(), shared_memory.json_buffer.begin() + shared_memory.message_length
            );
        }
        catch (const std::exception& e)
        {
            // Handle parsing errors
            fgc4::utils::Warning message(std::string("Error parsing JSON: ") + e.what() + std::string("\n"));
        }
        return json_object;
    }

}