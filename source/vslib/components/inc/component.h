//! @file
//! @brief File containing the common interface of all components.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include "componentRegistry.h"
#include "iparameter.h"
#include "nonCopyableNonMovable.h"
#include "parameterRegistry.h"
#include "staticJson.h"

namespace vslib::components
{
    class Component : public NonCopyableNonMovable
    {
        using ParameterReference = std::reference_wrapper<parameters::IParameter>;
        using ParameterList      = std::vector<std::tuple<std::string, ParameterReference>>;
        using ComponentReference = std::reference_wrapper<Component>;
        using ChildrenList       = std::vector<ComponentReference>;
        using StaticJson         = fgc4::utils::StaticJson;

      public:
        //! Creates the Component object with the provided type, name, and inside the hierarchy specified by parent
        //!
        //! @param component_type Type of the Component
        //! @param name Name of the Component, needs to be unique in the type
        //! @param parent Possible parent of this Component, if Component is independent, the parent should be nullptr
        Component(std::string_view component_type, std::string_view name, Component* parent) noexcept
            : m_component_type(component_type),
              m_name(name)
        {
            if (parent == nullptr)   // independent Component
            {
                registerComponent();
            }
            else
            {
                m_parent_name = std::string(parent->getFullName());
                parent->addChild((*this));
            }
        }

        //! Registers the parameter belonging to this component in the parameter registry, simultaneously adding it to
        //! the parameters vector.
        //!
        //! @param parameter_name Name of the parameter to be added to the parameter registry
        //! @param parameter Reference to the parameter being added to the parameter registry
        void registerParameter(std::string_view parameter_name, parameters::IParameter& parameter)
        {
            parameters::ParameterRegistry::instance().addToRegistry(
                this->getFullName() + "." + std::string(parameter_name), parameter
            );
            m_parameters.emplace_back(parameter_name, parameter);
        }

        //! Serializes this component to JSON, including all children components and parameters
        //! of the entire hierarchy.
        //!
        //! @return Returns a fully-serialized component as a JSON object
        [[nodiscard]] StaticJson serialize() const noexcept
        {
            StaticJson serialized_component  = nlohmann::json::array();
            StaticJson serialized_parameters = nlohmann::json::array();
            for (const auto& parameter : m_parameters)
            {
                serialized_parameters.emplace_back(std::get<1>(parameter).get().serialize());
            }

            StaticJson serialized_children = nlohmann::json::array();
            for (const auto& child : m_children)
            {
                serialized_children.emplace_back(child.get().serialize());
            }

            serialized_component
                = {{"name", m_name},
                   {"type", m_component_type},
                   {"parameters", serialized_parameters},
                   {"components", serialized_children}};
            return serialized_component;
        }

        //! Provides the name of this component
        //!
        //! @return String_view of the component name
        [[nodiscard]] std::string_view getName() const noexcept
        {
            return m_name;
        }

        //! Provides the full name of this component, including component
        //! type and parent name (if existing)
        //!
        //! @return String with the full component name
        [[nodiscard]] std::string getFullName() const noexcept
        {
            std::string full_name = m_component_type + "." + m_name;
            if (m_parent_name != "")
            {
                full_name = m_parent_name + "." + full_name;
            }
            return full_name;
        }

        //! Provides the map with all names and references to all parameters registered to this component
        //!
        //! @return Map with names and references to all parameters of this component
        [[nodiscard]] auto const& getParameters() const noexcept
        {
            return m_parameters;
        }

        void addChild(Component& child)
        {
            m_children.emplace_back(child);
        }

      protected:
        std::string const m_component_type;
        std::string       m_parent_name{""};
        std::string const m_name;
        ParameterList     m_parameters;
        ChildrenList      m_children;

        //! Registers this component in the ComponentRegistry
        void registerComponent() noexcept
        {
            ComponentRegistry::instance().addToRegistry(this->getFullName(), (*this));
        }
    };

    // ************************************************************
    // Constant for denoting that the component constructed is independent (has no parents)
    constexpr Component* independent_component = nullptr;

}   // namespace vslib::components
