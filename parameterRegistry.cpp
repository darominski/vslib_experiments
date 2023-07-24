//! @file
//! @brief Method definitions for the ParameterRegistry class.
//! @author Dominik Arominski

#include <ranges>
#include <type_traits>

#include "parameterRegistry.h"

using json = nlohmann::json;

namespace parameters
{

    using TypeToString = std::pair<Type, std::string_view>;
    constexpr std::array typeNames
        = {TypeToString{Type::Int32, "Int32"}, TypeToString{Type::Float32, "Float32"},
           TypeToString{Type::Float32Array, "Float32Array"}};

    static_assert(typeNames.size() == static_cast<size_t>(Type::Unsupported));

    constexpr std::string_view toString(Type type)
    {
        return std::ranges::find(typeNames, type, &TypeToString::first)->second;
    }

    void ParameterRegistry::addToRegistry(
        std::string_view parameter_name, std::tuple<VariableInfo, VariableInfo, VariableInfo>&& variable_info
    )
    {
        m_buffers.emplace(parameter_name, std::move(variable_info));
    }

    // ************************************************************

    //! Creates a JSON file manifest describing all settable parameters with their name IDs, memory address,
    //! and memory size, based on the information stored in the parameterRegistry.
    json ParameterRegistry::createManifest()
    {
        json       manifest;
        auto const write_registry    = this->getWriteAddressArray();
        auto const write_buffer_size = this->getBufferSize();
        std::for_each(
            std::cbegin(write_registry), std::cbegin(write_registry) + write_buffer_size,
            [&](const auto& address_element)
            {
                // finds first end of string character to properly size the name held in a too large array of chars
                auto const name = std::string(
                    std::begin(address_element.m_name),
                    std::find(std::begin(address_element.m_name), std::end(address_element.m_name), '\0')
                );
                // each parameter registry becomes a JSON file entry
                json json_entry
                    = {{"name", name},
                       {"memory_address", address_element.m_variable_info.memory_address},
                       {"size", address_element.m_variable_info.memory_size},
                       {"type", toString(address_element.m_variable_info.type)}};
                manifest.push_back(json_entry);
            }
        );

        return manifest;
    }
}