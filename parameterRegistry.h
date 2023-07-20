//! @file
//! @brief File definining registry with parameter addresses and the structure for the memory address entries.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

extern int buffer_switch;

namespace parameters
{
    // global constants defining variable sizes
    constexpr short  max_name_length   = 128;   // max length of component name, in characters
    constexpr size_t max_registry_size = 100;   // max number of possible settings

    // ************************************************************

    struct AddressEntry
    {
        AddressEntry(){};
        AddressEntry(std::string_view name, intptr_t address, size_t memory_size)
            : m_address(address),
              m_memory_size(memory_size)
        {
            std::fill(std::begin(m_name), std::end(m_name), '\0');
            size_t length = name.size();
            length        = length < max_name_length ? length : max_name_length - 1;
            std::copy(std::begin(name), std::begin(name) + length, std::begin(m_name));
            m_name[length] = '\0';
        };
        std::array<char, max_name_length> m_name;
        intptr_t                          m_address;
        size_t                            m_memory_size;
    };

    // ************************************************************

    class ParameterRegistry
    {
      public:
        // the registry shall not be assignable nor clonable
        ParameterRegistry(ParameterRegistry& other)                   = delete;
        void                      operator=(const ParameterRegistry&) = delete;
        static ParameterRegistry& instance()
        {
            // Registry is constructed on first access
            static ParameterRegistry m_instance;
            return m_instance;
        }

        void addToReadBufferRegistry(const std::string&, intptr_t, size_t);
        void addToWriteBufferRegistry(const std::string&, intptr_t, size_t);

        auto const& getBufferAddressArray()
        {
            return m_buffer_registry;
        }
        auto const& getWriteAddressArray()
        {
            return m_write_registry;
        }
        auto const getReadBufferSize() const
        {
            return m_read_buffer_size;
        }
        auto const getWriteBufferSize() const
        {
            return m_write_buffer_size;
        }

        nlohmann::json createManifest();

      private:
        ParameterRegistry(){};
        std::array<AddressEntry, max_registry_size> m_buffer_registry;
        std::array<AddressEntry, max_registry_size> m_write_registry;
        size_t                                      m_read_buffer_size{0};
        size_t                                      m_write_buffer_size{0};
    };
}   // namespace parameters