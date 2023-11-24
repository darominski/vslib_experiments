//! @file
//! @brief File defining all error codes used in the FGC4 namespace.
//! @author Dominik Arominski

#pragma once

#include <cstdint>

namespace fgc4::utils::errorCodes
{
    // arbitrary numbers for testing purposes
    constexpr uint32_t name_already_used = 1000;

    constexpr uint32_t allocation_buffer_overflow = 2000;

    constexpr uint32_t out_of_bounds_access = 3000;
}   // namespace fgc4::utils::errorCodes
