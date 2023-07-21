
#include <type_traits>

#include "parameterRegistry.h"

using json = nlohmann::json;

namespace parameters
{
    //! Adds a new entry to the read buffer registry (m_bufferRegistry) and increments the read buffer
    //! size.
    //!
    //! @param name Name of the new parameter.
    //! @param address Pointer containing the memory address of the parameter.
    //! @param memory_size Memory size of the new parameter.
    void ParameterRegistry::addToReadBufferRegistry(const std::string& name, VariableInfo&& variable_info)
    {
        if (m_read_buffer_size >= max_registry_size)
        {
            std::cerr << "ERROR! Read buffer overflow. Parameter: " << name << " discarted.\n";
            return;
        }
        m_buffer_registry[m_read_buffer_size] = AddressEntry(name, std::move(variable_info));
        m_read_buffer_size++;
    }

    // ************************************************************

    //! Adds a new entry to the write buffer registry (m_writeRegistry) and increments the write buffer
    //! size.
    //!
    //! @param name Name of the new parameter, needs to be unique.
    //! @param address Pointer containing the memory address of the parameter.
    //! @param memory_size Variable structure containing type of the new parameter and its memory size.
    void ParameterRegistry::addToWriteBufferRegistry(const std::string& name, VariableInfo&& variable_info)
    {
        if (m_write_buffer_size >= max_registry_size)
        {
            std::cerr << "ERROR! Write buffer overflow. Parameter: " << name << " discarted.\n";
            return;
        }
        // there should be no repeated names in the address structure communicated to a separate process
        for (size_t registry_index = 0; registry_index < m_write_buffer_size; registry_index++)
        {
            if (std::string(m_write_registry[registry_index].m_name.data()) == name)
            {
                std::cerr << "ERROR! Name: " << name << " already defined.\n";
                exit(1);
            }
        }
        m_write_registry[m_write_buffer_size] = AddressEntry(name, std::move(variable_info));
        m_write_buffer_size++;
    }

    // ************************************************************

    //! Creates a JSON file manifest describing all settable parameters with their name IDs, memory address,
    //! and memory size, based on the information stored in the parameterRegistry.
    json ParameterRegistry::createManifest()
    {
        json manifest;

        std::for_each(
            std::cbegin(m_write_registry), std::cbegin(m_write_registry) + m_write_buffer_size,
            [&](const auto& addressElement)
            {
                // finds first end of string character to properly size the name held in a too large array of chars
                auto const name = std::string(
                    std::begin(addressElement.m_name),
                    std::find(std::begin(addressElement.m_name), std::end(addressElement.m_name), '\0')
                );
                // each parameter registry becomes a JSON file entry
                json json_entry
                    = {{"name", name},
                       {"memory_address", addressElement.m_variable_info.memory_address},
                       {"size", addressElement.m_variable_info.memory_size}};
                manifest.push_back(json_entry);
            }
        );

        return manifest;
    }
}