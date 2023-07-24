//! @file
//! @brief Class defining self-registering parameters to define externally settable component parameters.
//! @author Dominik Arominski

#pragma once

#include <compare>
#include <string>

#include "parameterRegistry.h"

inline int buffer_switch = 0;

namespace parameters
{
    template<typename T>
    class Param
    {
      public:
        Param(std::string_view name, T value)
            : m_name(name),
              m_value{value, value, value}
        {
            registerParam();
        };
        // cannot clone Params
        Param& param(Param&) = delete;

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
            std::copy(std::begin(other.m_value), std::end(other.m_value), this->m_value);
        }

        std::partial_ordering operator<=>(const Param& other) const
        {
            // parameters are compared based on the values stored
            // in the currently active buffer
            auto const& lhs = this->m_value[buffer_switch];
            auto const& rhs = this.m_value[buffer_switch];
            if (lhs == rhs)
            {
                return std::partial_ordering::equivalent;
            }
            if (lhs > rhs)
            {
                return std::partial_ordering::less;
            }
            if (lhs < rhs)
            {
                return std::partial_ordering::greater;
            }
            // comparison reaches here if lhs or rhs are NaN or similar
            return std::partial_ordering::unordered;
        }

        [[nodiscard]] const T& value() const
        {
            return m_value[buffer_switch];
        }

        [[nodiscard]] const std::string& getName() const
        {
            return m_name;
        }

      private:
        const std::string
            m_name;   // Param ID indicating component type, its name and the variable name, should be unique
        T   m_value[3];

        void registerParam();
    };

    // ************************************************************

    //! Registers parameters in the parameter registry. It separates the buffer that is externally writable
    //! from the read buffers which are not set externally but only synchronised with the write buffer.
    template<typename T>
    void Param<T>::registerParam()
    {
        // it is sufficient to know only the address of the first element of the m_value buffer,
        // since the size of T is known, and he memory is assigned continuously,
        auto const first_address   = reinterpret_cast<intptr_t>(std::addressof(m_value[0]));
        auto constexpr memory_size = sizeof(T);
        auto type                  = getType<T>();

        ParameterRegistry::instance().addToRegistry(
            m_name,
            std::make_tuple(
                VariableInfo(type, first_address, memory_size),
                VariableInfo(type, first_address + memory_size, memory_size),
                VariableInfo(type, first_address + memory_size * 2, memory_size)
            )
        );
    }
}   // parameters namespace
