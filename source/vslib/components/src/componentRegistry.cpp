//! @file
//! @brief File containing definitions of component registry methods to add to the registry and serialize it.
//! @author Dominik Arominski

#include "component.h"
#include "componentRegistry.h"
#include "errorMessage.h"
#include "fmt/format.h"

using namespace nlohmann;

namespace vslib::components
{

    //! Adds a new entry to the component registry
    //!
    //! @param component_name Name of the component to be added to the component registry
    //! @param component_reference Reference to the component being added to the component registry
    void ComponentRegistry::addToRegistry(std::string_view component_name, Component& component_reference)
    {
        if (m_components.find(std::string(component_name)) != m_components.end())
        {
            fgc4::utils::Error error_message(
                std::string("Component name: ") + std::string(component_name)
                    + std::string(" already defined in the registry!\n"),
                fgc4::utils::constants::error_name_already_used
            );
            std::cerr << fmt::format("{}", error_message);
            throw std::runtime_error("Component name already exists!");
        }
        m_components.emplace(component_name, component_reference);
    }

    //! Creates a JSON file manifest describing all settable parameters with their name IDs, memory address,
    //! and memory size, based on the information stored in the parameterRegistry.
    //!
    //! @returns JSON object with all initialized components and their settable parameters.
    [[nodiscard("Manifest should not be discarded.")]] json ComponentRegistry::createManifest() const
    {
        json manifest = json::array();
        std::for_each(
            std::cbegin(this->m_components), std::cend(this->m_components),
            [&manifest](const auto& register_entry)
            {
                // each component registry entry becomes a JSON file entry
                auto const& component = register_entry.second.get();
                manifest.push_back(component.serialize());
            }
        );
        return manifest;
    }

}   // namespace components
