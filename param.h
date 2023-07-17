//! @file
//! @brief Class defining self-registering parameters to define externally settable component parameters.
//! @author Dominik Arominski

#pragma once

#include <iostream>
#include <string>

#include "addressRegistry.h"

inline int bufferSwitch = 0;

namespace parameters
{
    template<typename T>
    class Param
    {
      public:
        Param(const std::string& name, T value)
            : m_name(name),
              m_value{value, value, value}
        {
            registerParam();
        };
        // cannot assign nor clone Params
        Param& param(Param&)           = delete;
        void   operator=(const Param&) = delete;

        [[nodiscard]] const T& value() const
        {
            return m_value[bufferSwitch];
        }
        [[nodiscard]] const T* address(const short bufferId) const
        {
            return &m_value[bufferId];
        }

      private:
        const std::string m_name;
        T                 m_value[3];

        void registerParam();
    };

    // ************************************************************

    //! Registers parameters in the addressRegistry. It separates the buffer that is externally writable
    //! from the read buffers which are not set externally but only synchronised with the write buffer.
    template<typename T>
    void Param<T>::registerParam()
    {
        const auto            typeId = std::string(typeid(this->value()).name());
        addressRegistry::TYPE type;
        if (typeId == "d")
        {
            type = addressRegistry::TYPE::Float32;
        }
        else if (typeId == "i")
        {
            type = addressRegistry::TYPE::Int32;
        }
        else if (typeId == "St5arrayIdLm4EE")
        {
            type = addressRegistry::TYPE::ArrayFloat32;
        }
        else
        {
            std::cerr << "Type: " << typeId << " not supported.\n";
            exit(0);
        }
        // both read buffers to be written into the registry
        addressRegistry::AddressRegistry::instance().addToReadBufferRegistry(
            this->m_name, reinterpret_cast<intptr_t>(this->address(0)), addressRegistry::Variable(type, sizeof(T))
        );
        addressRegistry::AddressRegistry::instance().addToReadBufferRegistry(
            this->m_name, reinterpret_cast<intptr_t>(this->address(1)), addressRegistry::Variable(type, sizeof(T))
        );
        // and a write buffer
        addressRegistry::AddressRegistry::instance().addToWriteBufferRegistry(
            this->m_name, reinterpret_cast<intptr_t>(this->address(2)), addressRegistry::Variable(type, sizeof(T))
        );
    }
}   // Parameters namespace
