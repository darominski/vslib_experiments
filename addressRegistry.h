#pragma once

#include <array>
#include <string>

namespace addressRegistry
{
    // global constants defining variable sizes
    constexpr short  maxNameLength       = 128;   // max length of component name, in characters
    constexpr size_t addressRegistrySize = 100;   // max number of possible settings

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
            length        = length < name.size() ? length : maxNameLength - 1;
            std::copy(name.begin(), name.begin() + length, m_name.begin());
            m_name[length] = '\0';
        };
        std::array<char, 128> m_name{};
        intptr_t              m_addr;
        TYPE                  m_type;
    };

    class AddressRegistry {
        public:
            static AddressRegistry & instance() {
                //constructed on first access
                static AddressRegistry m_instance;
                return m_instance;
            }
            void addToRegistry(const std::string&, intptr_t, TYPE);

        std::array<AddressStruct, addressRegistrySize> m_addrRegistry;
        private:
            AddressRegistry() {};
            int m_registerCounter{0};
    };

    void AddressRegistry::addToRegistry(const std::string &name, intptr_t addr, TYPE type) {
        if (m_registerCounter >= addressRegistrySize)
        {
            m_registerCounter = 0;   // start over and begin overwriting or raise a warning/error?
        }
        m_addrRegistry[m_registerCounter] = AddressStruct(name, addr, type);
        m_registerCounter++;
    }
}