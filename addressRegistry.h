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

    struct Variable
    {
        TYPE   type;
        size_t size;
    };

    struct AddressStruct
    {
        AddressStruct(){};
        AddressStruct(const std::string& name, intptr_t addr, Variable type)
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
        Variable                          m_type;
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

        void addToReadBufferRegistry(const std::string&, intptr_t, Variable);
        void addToWriteBufferRegistry(const std::string&, intptr_t, Variable);

        auto const& getBufferAddrArray()
        {
            return m_bufferRegistry;
        }
        auto const& getWriteAddrArray()
        {
            return m_writeRegistry;
        }
        auto const getReadBufferSize() const
        {
            return m_readBufferSize;
        }
        auto const getWriteBufferSize() const
        {
            return m_writeBufferSize;
        }

      private:
        AddressRegistry(){};
        std::array<AddressStruct, max_registry_size> m_bufferRegistry;
        std::array<AddressStruct, max_registry_size> m_writeRegistry;
        size_t                                       m_readBufferSize{0};
        size_t                                       m_writeBufferSize{0};
    };

    void AddressRegistry::addToReadBufferRegistry(const std::string& name, intptr_t addr, Variable type)
    {
        if (m_readBufferSize >= max_registry_size)
        {
            m_readBufferSize = 0;   // start over and begin overwriting or raise a warning/error?
        }
        m_bufferRegistry[m_readBufferSize] = AddressStruct(name, addr, type);
        m_readBufferSize++;
    }

    void AddressRegistry::addToWriteBufferRegistry(const std::string& name, intptr_t addr, Variable type)
    {
        if (m_writeBufferSize >= max_registry_size)
        {
            m_writeBufferSize = 0;   // start over and begin overwriting or raise a warning/error?
        }
        // there should be no repeated names in the address structure communicated to a separate process
        for (size_t registerIndex = 0; registerIndex < m_writeBufferSize; registerIndex++)
        {
            if (std::string(m_writeRegistry[registerIndex].m_name.data()) == name)
            {
                std::cerr << "ERROR! Name: " << name << " already defined.\n";
                exit(1);
            }
        }
        m_writeRegistry[m_writeBufferSize] = AddressStruct(name, addr, type);
        m_writeBufferSize++;
    }
}