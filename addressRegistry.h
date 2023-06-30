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

    std::array<AddressStruct, addressRegistrySize> addrRegistry;
    static int                                     registerCounter = 0;

}