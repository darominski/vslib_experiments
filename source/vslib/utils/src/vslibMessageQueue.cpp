//! @file
//! @brief File containing implementation of methods used to manipulate communicate with the read/write message queues.
//! @author Dominik Arominski

#include "constants.hpp"
#include "errorCodes.hpp"
#include "errorMessage.hpp"
#include "fmt/format.h"
#include "vslibMessageQueue.hpp"
#include "warningMessage.hpp"

namespace vslib::utils
{
    void writeJsonToMessageQueue(
        const fgc4::utils::StaticJson& json_object, fgc4::utils::MessageQueueWriter<void>& message_queue
    )
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

    const fgc4::utils::StaticJson readJsonFromMessageQueue(std::span<uint8_t>& message)
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

    void writeStringToMessageQueue(const std::string& message, fgc4::utils::MessageQueueWriter<void>& message_queue)
    {
        if (message.size() < fgc4::utils::constants::string_memory_pool_size)
        {
            message_queue.write({(const uint8_t*)(message.data()), message.size()});
        }
        else
        {
            fgc4::utils::Error(
                "Error writing string to message queue: run out of shared memory.\n",
                fgc4::utils::errorCodes::allocation_buffer_overflow
            );
            throw std::bad_alloc();
        }
    }
}   // namespace vslib::utils
