#pragma once

#include <array>
#include <string>

extern int bufferSwitch;

namespace addressRegistry
{
    // global constants defining variable sizes
    constexpr short  max_name_length   = 128;   // max length of component name, in characters
    constexpr size_t max_registry_size = 100;   // max number of possible settings

    enum TYPE
    {
        Int32,
        Float32
    };

    struct AddressStruct
    {
        AddressStruct(){};
        AddressStruct(const std::string& name, intptr_t addr, TYPE type)
            : m_addr(addr),
              m_type(type)
        {
            size_t length = name.size();
            length        = length < name.size() ? length : max_name_length - 1;
            std::copy(name.begin(), name.begin() + length, m_name.begin());
            m_name[length] = '\0';
        };
        std::array<char, max_name_length> m_name{};
        intptr_t                          m_addr;
        TYPE                              m_type;
    };

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

        void addToReadBufferRegistry(const std::string&, intptr_t, TYPE);
        void addToWriteBufferRegistry(const std::string&, intptr_t, TYPE);

        auto const& getBufferAddrArray()
        {
            return m_bufferRegistry;
        }
        auto const& getWriteAddrArray()
        {
            return m_writeRegistry;
        }
        int getReadBufferSize() const
        {
            return m_readBufferSize;
        }
        int getWriteBufferSize() const
        {
            return m_writeBufferSize;
        }

      private:
        AddressRegistry(){};
        std::array<AddressStruct, max_registry_size> m_bufferRegistry;
        std::array<AddressStruct, max_registry_size> m_writeRegistry;
        int                                          m_readBufferSize{0};
        int                                          m_writeBufferSize{0};
    };

    void AddressRegistry::addToReadBufferRegistry(const std::string& name, intptr_t addr, TYPE type)
    {
        if (m_readBufferSize >= max_registry_size)
        {
            m_readBufferSize = 0;   // start over and begin overwriting or raise a warning/error?
        }
        m_bufferRegistry[m_readBufferSize] = AddressStruct(name, addr, type);
        m_readBufferSize++;
    }

    void AddressRegistry::addToWriteBufferRegistry(const std::string& name, intptr_t addr, TYPE type)
    {
        if (m_writeBufferSize >= max_registry_size)
        {
            m_writeBufferSize = 0;   // start over and begin overwriting or raise a warning/error?
        }
        m_writeRegistry[m_writeBufferSize] = AddressStruct(name, addr, type);
        m_writeBufferSize++;
    }
}