//! @file
//! @brief File containing the common interface of all components.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include "component.h"
#include "icomponent.h"
#include "iparameter.h"
#include "nonCopyableNonMovable.h"
#include "parameterRegistry.h"
#include "parameterSerializer.h"
#include "staticJson.h"

namespace vslib
{
    class RootComponent : public IComponent
    {

      public:
        //! Creates the RootComponent with type, name as the base of the hierarchy for Components.
        //!
        //! @param component_type Type of the RootComponent
        //! @param name Name of the RootComponent, needs to be unique in the type
        RootComponent(std::string_view component_type = "Root", std::string_view name = "root") noexcept
            : IComponent(component_type, name)
        {
            m_full_name = std::string(m_component_type) + "." + std::string(m_name);
        }
        // ************************************************************
        // Method for serializing this RootComponent

        //! Serializes this Component to JSON, including all children Components and Parameters
        //! across the entire hierarchy.
        //!
        //! @return Returns a fully-serialized Component as a JSON object
        [[nodiscard]] StaticJson serialize() const noexcept override
        {
            StaticJson serialized_component = nlohmann::json::object();

            StaticJson serialized_children = nlohmann::json::array();
            for (const auto& child : m_children)
            {
                serialized_children.emplace_back(child.get().serialize());
            }

            serialized_component
                = {{"name", m_name},
                   {"type", m_component_type},
                   {"parameters", nlohmann::json::array()},
                   {"components", serialized_children}};
            return serialized_component;
        }

        // ************************************************************

        //! Verifies parameters after they are set, to be called after parameters of this component are modified.
        //! The checks need to run on the inactive buffer values. No Parameters exist, so nothing to do.
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            return {};
        }
    };

}   // namespace vslib
