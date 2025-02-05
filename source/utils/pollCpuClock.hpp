//! @file
//! @brief File defining counter readers.
//! @author Dominik Arominski

#pragma once

#include <cstdint>

namespace fgc4::utils
{
    [[maybe_unused]] static uint64_t read_CNTPCT()
    {
        uint64_t cntval;
        // asm statement MUST be volatile, otherwise compiler will do weird, wrong things like coalescing the access
        asm volatile("mrs %0, CNTPCT_EL0" : "=r"(cntval));
        return cntval;
    }
}