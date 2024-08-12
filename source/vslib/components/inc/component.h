//! @file
//! @brief File containing the common interface of all components.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "icomponent.h"
#include "iparameter.h"
#include "nonCopyableNonMovable.h"
#include "parameterRegistry.h"
#include "parameterSerializer.h"
#include "staticJson.h"

namespace vslib
{
    class Component : public IComponent
    {

      public:
        //! Creates the Component object with the provided type, name, and inside the hierarchy specified by parent.
        //!
        //! @param component_type Type of the Component
        //! @param name Name of the Component, needs to be unique in the type
        //! @param parent Possible parent of this Component, for the root Component should be nullptr
        Component(std::string_view component_type, std::string_view name, IComponent& parent) noexcept
            : IComponent(component_type, name),
              m_parent(parent)
        {
            m_full_name = std::string(m_parent.get().getFullName()) + "." + std::string(m_component_type) + "."
                          + std::string(m_name);
            parent.addChild(*this);
        }

        // ************************************************************
        // Method for registering Parameters belonging to this Component and serializing it

        //! Registers the Parameter to this Component, adding it to the parameters vector and the registry.
        //!
        //! @param parameter Reference to the added Parameter
        void registerParameter(IParameter& parameter)
        {
            ParameterRegistry::instance().addToRegistry(
                std::string(this->getFullName()) + "." + std::string(parameter.getName()), parameter
            );
            m_parameters.emplace_back(parameter.getName(), parameter);
        }

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

            ParameterSerializer serializer;
            StaticJson          serialized_parameters = nlohmann::json::array();
            for (const auto& parameter : m_parameters)
            {
                serialized_parameters.emplace_back(serializer.serialize(std::get<1>(parameter).get()));
            }

            serialized_component
                = {{"name", m_name},
                   {"type", m_component_type},
                   {"parameters", serialized_parameters},
                   {"components", serialized_children}};
            return serialized_component;
        }

        // ************************************************************

        //! Verifies parameters after they are set, to be called after parameters of this component are modified.
        //! The checks need to run on the inactive buffer values.
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            return {};
        }

      protected:
        std::reference_wrapper<IComponent> m_parent;   //!< Parent of this Component
    };

}   // namespace vslib
