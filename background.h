//! @file
//! @brief File containing background-task specific code for copying and synchronising parameter buffers.
//! @author Dominik Arominski

#pragma once

#include <cstring>

#include "addressRegistry.h"

namespace backgroundTask
{
    //! Copies all contents of the currently used buffer to the background buffer to synchronise them.
    void synchroniseReadBuffers()
    {
        auto const& addressRegistrySize = addressRegistry::AddressRegistry::instance().getReadBufferSize();
        auto const& addressRegistry     = addressRegistry::AddressRegistry::instance().getBufferAddrArray();
        for (size_t currentAddress = 2 * (bufferSwitch ^ 1); currentAddress < addressRegistrySize; currentAddress += 2)
        {
            memcpy(
                reinterpret_cast<void*>(addressRegistry[currentAddress + (bufferSwitch ^ 1)].m_address),
                reinterpret_cast<void*>(addressRegistry[currentAddress + bufferSwitch].m_address),
                addressRegistry[currentAddress + bufferSwitch].m_memorySize
            );
        }
    }

    // ************************************************************

    //! Copies all contents of a write buffer to the background buffer, which is not currently used.
    void copyWriteBuffer()
    {
        auto const& addressRegistrySize      = addressRegistry::AddressRegistry::instance().getWriteBufferSize();
        auto const& writeBufferRegistry      = addressRegistry::AddressRegistry::instance().getWriteAddrArray();
        auto const& backgroundBufferRegistry = addressRegistry::AddressRegistry::instance().getBufferAddrArray();

        for (size_t currentAddress = 0; currentAddress < addressRegistrySize; currentAddress++)
        {
            auto const  backgroundBufferAddress = 2 * currentAddress + (bufferSwitch ^ 1);
            auto const& targetBufferAddress     = backgroundBufferRegistry[backgroundBufferAddress]
                                                  .m_address;   // only bkg buffer elements are modified
            auto const& writeBufferAddress
                = writeBufferRegistry[currentAddress].m_address;   // each write buffer element is visited
            memcpy(
                reinterpret_cast<void*>(targetBufferAddress), reinterpret_cast<void*>(writeBufferAddress),
                backgroundBufferRegistry[backgroundBufferAddress].m_memorySize
            );
        }
    }
}