//! @file
//! @brief File containing definitions of component registry methods used to add elements to the registry and serialize
//! them.
//! @author Dominik Arominski

#include "component.h"
#include "componentRegistry.h"
#include "errorCodes.h"
#include "errorMessage.h"
#include "fmt/format.h"

using namespace nlohmann;
using namespace fgc4::utils;

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
            Error error_message(
                std::string("Component name: ") + std::string(component_name)
                    + std::string(" already defined in the registry!\n"),
                errorCodes::name_already_used
            );
            std::cerr << fmt::format("{}", error_message);
            throw std::runtime_error("Component name already exists!");
        }
        m_components.emplace(component_name, component_reference);
    }

    //! Creates a JSON manifest describing all settable parameters with their name, type, value,
    //! limits, and possibly allowed values in case of enumeration, all based on the information stored in the
    //! parameterRegistry.
    //!
    //! @returns JSON object with all initialized components and their settable parameters.
    [[nodiscard("Manifest should not be discarded.")]] json ComponentRegistry::createManifest() const
    {
        json manifest = json::array();
        for (const auto& component : m_components)
        {
            manifest.push_back(component.second.get().serialize());
        }
        return manifest;
    }
}   // namespace components
