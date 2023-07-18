//! @file
//! @brief File definining address registry singleton class and the structure of the memory address entries.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

extern int buffer_switch;

namespace parameters
{
    // global constants defining variable sizes
    constexpr short  max_name_length   = 128;   // max length of component name, in characters
    constexpr size_t max_registry_size = 100;   // max number of possible settings

    // ************************************************************

    struct AddressStruct
    {
        AddressStruct(){};
        AddressStruct(const std::string& name, intptr_t address, size_t memory_size)
            : m_address(address),
              m_memory_size(memory_size)
        {
            size_t length = name.size();
            length        = length < name.size() ? length : max_name_length - 1;
            std::copy(name.begin(), name.begin() + length, m_name.begin());
            m_name[length] = '\0';
        };
        std::array<char, max_name_length> m_name{};
        intptr_t                          m_address;
        size_t                            m_memory_size;
    };

    // ************************************************************

    class AddressRegistry
    {
      public:
        // the registry shall not be assignable nor clonable
        AddressRegistry(AddressRegistry& other)                   = delete;
        void                    operator=(const AddressRegistry&) = delete;
        static AddressRegistry& instance()
        {
            // Registry is constructed on first access
            static AddressRegistry m_instance;
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

      private:
        AddressRegistry(){};
        std::array<AddressStruct, max_registry_size> m_buffer_registry;
        std::array<AddressStruct, max_registry_size> m_write_registry;
        size_t                                       m_read_buffer_size{0};
        size_t                                       m_write_buffer_size{0};
    };

    // ************************************************************

    //! Adds a new entry to the read buffer registry (m_bufferRegistry) and increments the read buffer
    //! size.
    //!
    //! @param name Name of the new parameter.
    //! @param address Pointer containing the memory address of the parameter.
    //! @param memory_size Memory size of the new parameter.
    void AddressRegistry::addToReadBufferRegistry(const std::string& name, intptr_t address, size_t memory_size)
    {
        if (m_read_buffer_size >= max_registry_size)
        {
            std::cerr << "ERROR! Read buffer overflow. Parameter: " << name << " discarted.\n";
            return;
        }
        m_buffer_registry[m_read_buffer_size] = AddressStruct(name, address, memory_size);
        m_read_buffer_size++;
    }

    // ************************************************************

    //! Adds a new entry to the write buffer registry (m_writeRegistry) and increments the write buffer
    //! size.
    //!
    //! @param name Name of the new parameter, needs to be unique.
    //! @param address Pointer containing the memory address of the parameter.
    //! @param memory_size Variable structure containing type of the new parameter and its memory size.
    void AddressRegistry::addToWriteBufferRegistry(const std::string& name, intptr_t address, size_t memory_size)
    {
        if (m_write_buffer_size >= max_registry_size)
        {
            std::cerr << "ERROR! Write buffer overflow. Parameter: " << name << " discarted.\n";
            return;
        }
        // there should be no repeated names in the address structure communicated to a separate process
        for (size_t registry_index = 0; registry_index < m_write_buffer_size; registry_index++)
        {
            if (std::string(m_write_registry[registry_index].m_name.data()) == name)
            {
                std::cerr << "ERROR! Name: " << name << " already defined.\n";
                exit(1);
            }
        }
        m_write_registry[m_write_buffer_size] = AddressStruct(name, address, memory_size);
        m_write_buffer_size++;
    }
}   // namespace parameters