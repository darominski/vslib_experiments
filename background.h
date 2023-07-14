#pragma once

#include <cstring>

#include "addressRegistry.h"

namespace bkgTask
{
    /**
     * Copies all contents of the currently used buffer to the background buffer to synchronise them.
     */
    void synchroniseReadBuffers()
    {
        auto const& addrRegistrySize = addressRegistry::AddressRegistry::instance().getReadBufferSize();
        auto const& addressRegistry  = addressRegistry::AddressRegistry::instance().getBufferAddrArray();
        for (size_t iter = 2 * (bufferSwitch ^ 1); iter < addrRegistrySize; iter += 2)
        {
            memcpy(
                reinterpret_cast<void*>(addressRegistry[iter + (bufferSwitch ^ 1)].m_addr),
                reinterpret_cast<void*>(addressRegistry[iter + bufferSwitch].m_addr),
                sizeof(addressRegistry[iter + bufferSwitch].m_addr)
                // TODO: better way to get a size of this memory block, currently fetching intptr_t pointer size
            );
        }
    }

    /**
     * Copies all contents of a write buffer to the background buffer, which is not currently used.
     */
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
                sizeof(writeBufferAddr)
                // TODO: better way to get a size of this memory block, currently fetching intptr_t pointer size
            );
        }
    }
}