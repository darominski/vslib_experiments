//! @file
//! @brief File containing implementation of methods used to manipulate the shared memory object.
//! @author Dominik Arominski

#include "sharedMemory.h"
#include "warningMessage.h"

namespace vslib
{
    //! Helper function to serialize JSON object and write to the message queue
    //!
    //! @param json_object JSON object to be copied to the shared memory
    //! @param message_queue Reference to the shared memory object
    void writeJsonToMessageQueue(
        const fgc4::utils::StaticJson& json_object, bmboot::MessageQueueWriter<SharedMemory>& message_queue
    )
    {
        auto serialized = json_object.dump();
        if (serialized.size() < fgc4::utils::constants::json_memory_pool_size)
        {
            SharedMemory                   shared_memory(serialized.size());
            std::span<const unsigned char> serialized_message(
                reinterpret_cast<const unsigned char*>(serialized.data()), serialized.size()
            );
            message_queue.write(shared_memory, serialized_message);
        }
        else
        {
            fgc4::utils::Error(
                "Error writing JSON: run out of shared memory.\n", fgc4::utils::errorCodes::allocation_buffer_overflow
            );
            throw std::bad_alloc();
        }
    }

    //! Helper function to read JSON object from the message and deserialize it
    //!
    //! @param message Reference to the shared memory object
    //! @return Static JSON object parsed from shared memory
    fgc4::utils::StaticJson readJsonFromMessageQueue(const std::pair<SharedMemory, std::span<uint8_t>>& message)
    {
        auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
        try
        {
            json_object
                = nlohmann::json::parse(message.second.begin(), message.second.begin() + message.first.message_length);
        }
        catch (const std::exception& e)
        {
            // Handle parsing errors
            fgc4::utils::Warning warning(std::string("Error parsing JSON: ") + e.what() + std::string("\n"));
        }
        return json_object;
    }

}
