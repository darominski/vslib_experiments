//! @file
//! @brief File definining registry with parameter addresses and the structure for the memory address entries.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

extern int buffer_switch;

namespace parameters
{
    // global constants defining variable sizes
    constexpr short  max_name_length   = 128;   // max length of component name, in characters
    constexpr size_t max_registry_size = 100;   // max number of possible settings

    enum class Type
    {
        Int32,
        Float32,
        Float32Array,
        Unsupported   // must be last
    };

    // helper definitions for std::array types
    template<typename T>
    struct is_std_array : std::false_type
    {
    };

    template<typename T, std::size_t N>
    struct is_std_array<std::array<T, N>> : std::true_type
    {
    };

    template<class... T>
    constexpr bool always_false = false;

    // if the body of getType is in the cpp file, certain types (double, array)
    // are not created. Leaving here for now.
    //! Returns the type of the parameter added to the registry.
    template<typename T>
    constexpr Type getType()
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return Type::Float32;
        }
        else if constexpr (std::is_integral<T>::value)
        {
            return Type::Int32;
        }
        else if constexpr (is_std_array<T>::value)
        {
            using ElementType = typename T::value_type;
            if (std::is_floating_point_v<ElementType>)
            {
                return Type::Float32Array;
            }
        }
        else
        {
            static_assert(always_false<T>, "Unsupported type.");
        }
        // it should never reach here
        return Type::Unsupported;
    };

    constexpr std::string_view toString(Type type);

    // ************************************************************
    // structure holding all information about a stored variable necessary for remote setting
    struct VariableInfo
    {
        Type     type;
        intptr_t memory_address;
        size_t   memory_size;
    };

    // ************************************************************

    struct AddressEntry
    {
        AddressEntry(){};
        AddressEntry(std::string_view name, VariableInfo variable_info)
            : m_variable_info(variable_info)
        {
            std::fill(std::begin(m_name), std::end(m_name), '\0');
            size_t length = name.size();
            length        = length < max_name_length ? length : max_name_length - 1;
            std::copy(std::begin(name), std::begin(name) + length, std::begin(m_name));
            m_name[length] = '\0';
        };
        std::array<char, max_name_length> m_name;
        VariableInfo                      m_variable_info;
    };

    // ************************************************************

    class ParameterRegistry
    {
      public:
        // the registry shall not be assignable nor clonable
        ParameterRegistry(ParameterRegistry& other)                   = delete;
        void                      operator=(const ParameterRegistry&) = delete;
        static ParameterRegistry& instance()
        {
            // Registry is constructed on first access
            static ParameterRegistry m_instance;
            return m_instance;
        }

        void addToReadBufferRegistry(const std::string&, VariableInfo&&);
        void addToWriteBufferRegistry(const std::string&, VariableInfo&&);

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

        nlohmann::json createManifest();

      private:
        ParameterRegistry(){};
        std::array<AddressEntry, max_registry_size> m_buffer_registry;
        std::array<AddressEntry, max_registry_size> m_write_registry;
        size_t                                      m_read_buffer_size{0};
        size_t                                      m_write_buffer_size{0};
    };
}   // namespace parameters