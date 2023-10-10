//! @file
//! @brief File defining static heap allocation for working with JSON objects.
//! @author Dariusz Zielinski

#pragma once

#include "constants.h"
#include "nlohmann/json.hpp"
#include "ringBufferAllocator.h"

namespace fgc4::utils
{
    // ************************************************************
    // Helper definition for the verbose template names

    class StaticJsonFactory;   // forward declaration of StaticJsonFactory

    template<typename T>
    using JsonAllocator = StaticRingBufferAllocator<T, StaticJsonFactory, constants::json_memory_pool_size>;

    using StaticJson = nlohmann::basic_json<
        nlohmann::ordered_map, std::vector, std::string, bool, std::int64_t, std::uint64_t, double, JsonAllocator>;

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
