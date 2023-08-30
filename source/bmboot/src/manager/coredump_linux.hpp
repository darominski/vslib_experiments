//! @file
//! @brief  Core dump generation
//! @author Martin Cejp

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "../cpu_state.hpp"

namespace bmboot::internal
{

    struct MemorySegment
    {
        size_t      start_address, size;
        void const* ptr;
    };

    void writeCoreDump(
        char const* fn, std::span<MemorySegment const> segments, Aarch64_Regs const& the_regs,
        Aarch64_FpRegs const& fpregs
    );

}
