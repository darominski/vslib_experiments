//! @file
//! @brief Class defining self-registering parameters to define externally settable component parameters.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "parameterRegistry.h"

inline int buffer_switch = 0;

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
        // cannot clone Params
        Param& param(Param&) = delete;

        [[nodiscard]] const T& value() const
        {
            return m_value[buffer_switch];
        }
        [[nodiscard]] const T* address(const short buffer_id) const
        {
            if (buffer_id < 0 || buffer_id > 2)
            {
                std::cerr << "Error! Incorrect buffer_id: " << buffer_id << ". Allowed values are: 0, 1, 2.\n";
                return nullptr;
            }
            return &m_value[buffer_id];
        }

        operator T() const
        {
            return m_value[buffer_switch];
        }

        operator T&()
        {
            return m_value[buffer_switch];
        }

        void operator=(const Param& other)
        {
            std::copy(this->m_value, other.m_value);
        }


      private:
        const std::string m_name;
        T                 m_value[3];

        void registerParam();
    };

    // ************************************************************

    //! Registers parameters in the parameter registry. It separates the buffer that is externally writable
    //! from the read buffers which are not set externally but only synchronised with the write buffer.
    template<typename T>
    void Param<T>::registerParam()
    {
        // both read buffers to be written into the registry
        parameters::ParameterRegistry::instance().addToReadBufferRegistry(
            this->m_name, reinterpret_cast<intptr_t>(this->address(0)), sizeof(T)
        );
        parameters::ParameterRegistry::instance().addToReadBufferRegistry(
            this->m_name, reinterpret_cast<intptr_t>(this->address(1)), sizeof(T)
        );
        // and a write buffer
        parameters::ParameterRegistry::instance().addToWriteBufferRegistry(
            this->m_name, reinterpret_cast<intptr_t>(this->address(2)), sizeof(T)
        );
    }
}   // parameters namespace
