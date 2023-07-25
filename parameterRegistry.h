//! @file
//! @brief File definining registry with parameter addresses and the structure for the memory address entries.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <array>
#include <string>
#include <tuple>

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

    // ************************************************************

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

    using TypeToString = std::pair<Type, std::string_view>;
    constexpr std::array type_names
        = {TypeToString{Type::Int32, "Int32"}, TypeToString{Type::Float32, "Float32"},
           TypeToString{Type::Float32Array, "Float32Array"}};

    static_assert(type_names.size() == static_cast<size_t>(Type::Unsupported));

    constexpr extern std::string_view toString(Type type)
    {
        return std::ranges::find(type_names, type, &TypeToString::first)->second;
    }

    constexpr extern Type fromString(std::string_view type_string)
    {
        return std::ranges::find(type_names, type_string, &TypeToString::second)->first;
    }

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

        void addToRegistry(std::string_view, std::tuple<VariableInfo, VariableInfo, VariableInfo>&&);

        auto const getWriteAddressArray() const
        {
            std::array<AddressEntry, max_registry_size> write_address_array;
            int                                         element_counter = 0;
            std::for_each(
                std::cbegin(m_buffers), std::cend(m_buffers),
                [&](const auto& map_element)
                {
                    write_address_array[element_counter]
                        = AddressEntry(map_element.first, std::get<2>(map_element.second));
                    element_counter++;
                }
            );
            return write_address_array;
        }

        auto const getBufferSize() const
        {
            return m_buffers.size();
        }

        auto const& getBuffers() const
        {
            return m_buffers;
        }

        nlohmann::json createManifest();

      private:
        ParameterRegistry(){};
        std::map<std::string, std::tuple<VariableInfo, VariableInfo, VariableInfo>> m_buffers;
    };
}   // namespace parameters