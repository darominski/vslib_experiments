//! @file
//! @brief File containing background-task specific code for copying and synchronising parameter buffers.
//! @author Dominik Arominski

#pragma once

#include <cstring>

#include "parameterRegistry.h"

namespace backgroundTask
{
    void synchroniseReadBuffers();
    void copyWriteBuffer();

    //! Copies all contents of the currently used buffer to the background buffer to synchronise them.
    void synchroniseReadBuffers()
    {
        auto const& address_registry_size = parameters::ParameterRegistry::instance().getReadBufferSize();
        auto const& address_registry      = parameters::ParameterRegistry::instance().getBufferAddressArray();
        for (size_t current_address = 2 * (buffer_switch ^ 1); current_address < address_registry_size;
             current_address        += 2)
        {
            memcpy(
                reinterpret_cast<void*>(address_registry[current_address + (buffer_switch ^ 1)].m_address),
                reinterpret_cast<void*>(address_registry[current_address + buffer_switch].m_address),
                address_registry[current_address + buffer_switch].m_memory_size
            );
        }
    }

    // ************************************************************

    //! Copies all contents of a write buffer to the background buffer, which is not currently used.
    void copyWriteBuffer()
    {
        auto const& address_registry_size      = parameters::ParameterRegistry::instance().getWriteBufferSize();
        auto const& write_buffer_registry      = parameters::ParameterRegistry::instance().getWriteAddressArray();
        auto const& background_buffer_registry = parameters::ParameterRegistry::instance().getBufferAddressArray();

        for (size_t current_address = 0; current_address < address_registry_size; current_address++)
        {
            auto const  background_buffer_address = 2 * current_address + (buffer_switch ^ 1);
            auto const& target_buffer_address     = background_buffer_registry[background_buffer_address]
                                                    .m_address;   // only bkg buffer elements are modified
            auto const& write_buffer_address
                = write_buffer_registry[current_address].m_address;   // each write buffer element is visited
            memcpy(
                reinterpret_cast<void*>(target_buffer_address), reinterpret_cast<void*>(write_buffer_address),
                background_buffer_registry[background_buffer_address].m_memory_size
            );
        }
    }
}   // namespace backgroundTask