//! @file
//! @brief File defining static heap allocation for working with JSON objects.
//! @author Dariusz Zielinski

#pragma once

#include "constants.h"
#include "nlohmann/json.hpp"
#include "ringBufferAllocator.h"

namespace vslib::utils
{
    // ************************************************************
    // Helper definition for the verbose template names

    class StaticJsonFactory;   // forward declaration of StaticJsonFactory

    template<typename T>
    using JsonAllocator = StaticRingBufferAllocator<T, StaticJsonFactory, constants::json_memory_pool_size>;

    using StaticJson = nlohmann::basic_json<
        std::map, std::vector, std::string, bool, std::int64_t, std::uint64_t, double, JsonAllocator>;

    // ************************************************************
    // Initialization of static heaps for supported types

    // Let's align the buffers to the largest type we want to have in our JSON
    template<typename BufferType, size_t BufferSize>
    alignas(std::max_align_t) std::byte RingBuffer<BufferType, BufferSize>::m_buffer[];

    template<typename BufferType, size_t BufferSize>
    size_t RingBuffer<BufferType, BufferSize>::m_current_position = 0;

    // ************************************************************

    class StaticJsonFactory
    {
      public:
        static StaticJson getJsonObject()
        {
            RingBuffer<StaticJsonFactory, constants::json_memory_pool_size>::clear();
            return {};   // This is important, to always get the fresh object
        }
    };
}   // namespace utils