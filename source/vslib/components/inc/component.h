//! @file
//! @brief File containing the common interface of all components to be reused.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include "componentRegistry.h"
#include "iparameter.h"
#include "nlohmann/json.hpp"
#include "parameterRegistry.h"

namespace vslib::components
{
    class Component
    {
      public:
        // For consistency of behaviour around registration of Components and their Parameters,
        // a number of constructors are explicitly deleted
        Component()                             = delete;   // anonymous Components are forbidden
        Component(Component&)                   = delete;   // cloning objects is forbidden
        Component(Component&&)                  = delete;   // moving objects is forbidden
        Component& operator=(const Component&)  = delete;   // copy-assignment is forbidden
        Component& operator=(const Component&&) = delete;   // move-assignment is forbidden

        // ************************************************************

        Component(std::string_view component_type, std::string_view name, Component* parent) noexcept
            : m_component_type(component_type),
              m_name(name)
        {
            if (parent)
            {
                m_parent_name = std::string(parent->getFullName());
                parent->addChild((*this));
            }
            else
            {
                registerComponent();
            }
        }

        virtual ~Component() = default;

        //! Registers the parameter belonging to this component in the parameter registry, simultaneously adding it to
        //! the m_parameters vector.
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

        //! Registers this component in the ComponentRegistry
        void registerComponent() noexcept
        {
            ComponentRegistry::instance().addToRegistry(m_parent_name + m_name, (*this));
        }

        //! Serializes this component to JSON, including all children components and parameters
        //! of the entire hierarchy.
        //!
        //! @return Returns a fully-serialized component as a JSON object
        nlohmann::json serialize() const noexcept
        {
            nlohmann::json serialized_parameters = nlohmann::json::array();
            std::for_each(
                std::cbegin(m_parameters), std::cend(m_parameters),
                [&serialized_parameters](const auto& parameter)
                {
                    serialized_parameters.emplace_back(std::get<1>(parameter).get().serialize());
                }
            );

            nlohmann::json serialized_children = nlohmann::json::array();
            std::for_each(
                std::cbegin(m_children), std::cend(m_children),
                [&serialized_children](const auto& child)
                {
                    serialized_children.emplace_back(child.get().serialize());
                }
            );

            return {
                {"name", m_name},
                {"type", m_component_type},
                {"parameters", serialized_parameters},
                {"components", serialized_children}};
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

      private:
        std::string const                                                                    m_component_type;
        std::string                                                                          m_parent_name{""};
        std::string const                                                                    m_name;
        std::vector<std::tuple<std::string, std::reference_wrapper<parameters::IParameter>>> m_parameters;
        std::vector<std::reference_wrapper<Component>>                                       m_children;
    };

    // ************************************************************
    // Constant for denoting that the component constructed is independent (has no parents)
    constexpr components::Component* independent_component = nullptr;

}   // namespace component
