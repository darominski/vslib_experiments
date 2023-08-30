//! @file
//! @brief Class providing an alias for LogString to be used in all logs. This definition makes sure a fixed-size memory
//! pool is used, preventing memory fragmentation.
//! @author Dominik Arominski

#pragma once

#include "constants.h"
#include "ringBufferAllocator.h"

namespace vslib::utils
{
    class LogStringType;   // empty class for defining dedicated HeapType

    using StringAllocator = StaticRingBufferAllocator<char, LogStringType, constants::string_memory_pool_size>;

    using LogString = std::basic_string<char, std::char_traits<char>, StringAllocator>;

}   // namespace utils