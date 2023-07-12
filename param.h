#pragma once

#include <iostream>
#include <string>

#include "addressRegistry.h"

namespace Parameters
{

    template<typename T>
    class Param
    {
      public:
        Param(const std::string& name, T value)
            : m_name(name),
              m_value(value)
        {
            registerParam();
        };
        // cannot assign nor clone Params
        Param& param(Param&)           = delete;
        void   operator=(const Param&) = delete;

        [[nodiscard]] const T& value() const
        {
            return m_value;
        }
        [[nodiscard]] const T* address() const
        {
            return &m_value;
        }

      private:
        const std::string m_name;
        T                 m_value;

        void registerParam();
    };

    template<typename T>
    void Param<T>::registerParam()
    {
        const auto            typeId = typeid(this->value()).name();
        addressRegistry::TYPE type;
        if (type == 'd')
        {
            type = addressRegistry::TYPE::Float32;
        }
        else if (type == 'i')
        {
            type = addressRegistry::TYPE::Int32;
        }
        else
        {
            // ERR...
        }
        addressRegistry::AddressRegistry::instance().addToRegistry(
            this->m_name, reinterpret_cast<intptr_t>(this->address()), type
        );
    }
}   // Parameters namespace
