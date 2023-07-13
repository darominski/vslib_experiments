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
        void        addToRegistry(const std::string&, intptr_t, TYPE);
        auto const& getAddrArray()
        {
            return m_addrRegistry;
        }
        int getRegisterCounter() const
        {
            return m_registerCounter;
        }

      private:
        AddressRegistry(){};
        std::array<AddressStruct, max_registry_size> m_addrRegistry;
        int                                          m_registerCounter{0};
    };

    void AddressRegistry::addToRegistry(const std::string& name, intptr_t addr, TYPE type)
    {
        if (m_registerCounter >= max_registry_size)
        {
            m_registerCounter = 0;   // start over and begin overwriting or raise a warning/error?
        }
        m_addrRegistry[m_registerCounter] = AddressStruct(name, addr, type);
        m_registerCounter++;
    }
}