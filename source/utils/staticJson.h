//! @file
//! @brief File defining static heap allocation for working with JSON objects.
//! @author Dariusz Zielinski

#pragma once

#include "constants.h"
#include "json/json.hpp"
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
            // TO-DO: This buffer used to be cleared before each use. A better approach than just letting it loop
            // around with possibly many objects in the memory needs to be found. More dynamic memory allocation
            // along with separate buffers for different uses should be investigated.
            return {};   // This is important, to always get the fresh object
        }
    };
}   // namespace fgc4::utils
