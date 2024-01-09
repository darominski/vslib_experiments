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
    void
    writeJsonToMessageQueue(const fgc4::utils::StaticJson& json_object, bmboot::MessageQueueWriter<void>& message_queue)
    {
        auto serialized = json_object.dump();
        if (serialized.size() < fgc4::utils::constants::json_memory_pool_size)
        {
            message_queue.write({reinterpret_cast<uint8_t*>(serialized.data()), serialized.size()});
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
    fgc4::utils::StaticJson readJsonFromMessageQueue(std::span<uint8_t>& message)
    {
        auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
        try
        {
            json_object = nlohmann::json::parse(message.begin(), message.end());
        }
        catch (const std::exception& e)
        {
            // Handle parsing errors
            fgc4::utils::Warning warning(std::string("Error parsing JSON: ") + e.what() + std::string("\n"));
        }
        return json_object;
    }

}
