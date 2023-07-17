//! @file
//! @brief File containing background-task specific code for copying and synchronising parameter buffers.
//! @author Dominik Arominski

#pragma once

#include <cstring>

#include "addressRegistry.h"

namespace bkgTask
{
    //! Copies all contents of the currently used buffer to the background buffer to synchronise them.
    void synchroniseReadBuffers()
    {
        auto const& addrRegSize = addressRegistry::AddressRegistry::instance().getReadBufferSize();
        auto const& addressReg  = addressRegistry::AddressRegistry::instance().getBufferAddrArray();
        for (size_t iter = 2 * (bufferSwitch ^ 1); iter < addrRegSize; iter += 2)
        {
            memcpy(
                reinterpret_cast<void*>(addressReg[iter + (bufferSwitch ^ 1)].m_addr),
                reinterpret_cast<void*>(addressReg[iter + bufferSwitch].m_addr),
                addressReg[iter + bufferSwitch].m_type.size
            );
        }
    }

    // ************************************************************

    //! Copies all contents of a write buffer to the background buffer, which is not currently used.
    void copyWriteBuffer()
    {
        auto const& addrRegistrySize    = addressRegistry::AddressRegistry::instance().getWriteBufferSize();
        auto const& writeBufferRegistry = addressRegistry::AddressRegistry::instance().getWriteAddrArray();
        auto const& bkgBufferRegistry   = addressRegistry::AddressRegistry::instance().getBufferAddrArray();

        for (size_t iter = 0; iter < addrRegistrySize; iter++)
        {
            auto const& targetBufferAddr
                = bkgBufferRegistry[2 * iter + (bufferSwitch ^ 1)].m_addr;    // only bkg buffer elements are modified
            auto const& writeBufferAddr = writeBufferRegistry[iter].m_addr;   // each write buffer element is visited
            memcpy(
                reinterpret_cast<void*>(targetBufferAddr), reinterpret_cast<void*>(writeBufferAddr),
                bkgBufferRegistry[2 * iter + (bufferSwitch ^ 1)].m_type.size
            );
        }
    }
}