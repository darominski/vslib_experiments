//! @file
//! @brief File containing the common interface of all components.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include "iparameter.h"
#include "nonCopyableNonMovable.h"
#include "parameterRegistry.h"
#include "parameterSerializer.h"
#include "staticJson.h"

namespace vslib
{
    class Component : public NonCopyableNonMovable
    {
        using ParameterReference = std::reference_wrapper<IParameter>;
        using ParameterList      = std::vector<std::pair<std::string, ParameterReference>>;
        using ComponentReference = std::reference_wrapper<Component>;
        using ChildrenList       = std::vector<ComponentReference>;
        using StaticJson         = fgc4::utils::StaticJson;

      public:
        //! Creates the Component object with the provided type, name, and inside the hierarchy specified by parent
        //!
        //! @param component_type Type of the Component
        //! @param name Name of the Component, needs to be unique in the type
        //! @param parent Possible parent of this Component, if Component is independent, the parent should be nullptr
        Component(std::string_view component_type, std::string_view name, Component* parent = nullptr) noexcept
            : m_component_type(component_type),
              m_name(name),
              m_parent(parent)
        {
            if (parent != nullptr)
            {
                parent->addChild((*this));
            }
        }

        // ************************************************************
        // Methods for registering Parameters belonging to this Component and serializing it

        //! Adds a child Component to this Component
        //!
        //! @param child Child Component to be added to this Component
        void addChild(Component& child)
        {
            m_children.emplace_back(child);
        }

        //! Registers the parameter belonging to this component in the parameter registry, simultaneously adding it to
        //! the parameters vector.
        //!
        //! @param parameter_name Name of the parameter to be added to the parameter registry
        //! @param parameter Reference to the parameter being added to the parameter registry
        void registerParameter(std::string_view parameter_name, IParameter& parameter)
        {
            ParameterRegistry::instance().addToRegistry(
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
            StaticJson          serialized_component  = nlohmann::json::array();
            StaticJson          serialized_parameters = nlohmann::json::array();
            ParameterSerializer serializer;
            for (const auto& parameter : m_parameters)
            {
                serialized_parameters.emplace_back(serializer.serialize(std::get<1>(parameter).get()));
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

        // ************************************************************
        // Getters

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
            const std::string full_name = m_component_type + "." + m_name;
            return (m_parent == nullptr) ? full_name : m_parent->getFullName() + "." + full_name;
        }

        //! Provides the map with all names and references to all parameters registered to this component
        //!
        //! @return Map with names and references to all parameters of this component
        [[nodiscard]] auto const& getParameters() const noexcept
        {
            return m_parameters;
        }

        //! Provides the container with all children belonging to this component
        //!
        //! @return Vector with references to all children of this component
        [[nodiscard]] auto const& getChildren() const noexcept
        {
            return m_children;
        }

        //! Provides information whether this Component has a parent
        //!
        //! @return True if a parent is defined, false otherwise
        [[nodiscard]] bool hasParent() const noexcept
        {
            return (m_parent != nullptr);
        }

        //! Returns the value of the flag informing whether the parameters of this Component have been recently modified
        //!
        //! @return True if any parameter of this Component has been recently modified, false otherwise
        [[nodiscard]] bool parametersInitialized() const noexcept
        {
            for (const auto& parameter : m_parameters)
            {
                if (!parameter.second.get().isInitialized())
                {
                    return false;
                    break;
                }
            }
            return true;
        }

        // ************************************************************
        // Miscellaneous methods, interaction with buffers and verifying parameters

        //! Flips the buffer state of all Parameters registered with this Component
        void flipBufferState() noexcept
        {
            for (auto& parameter : m_parameters)
            {
                parameter.second.get().swapBuffers();
            }
        }

        //! Synchronises buffers for all Parameters registered with this Component
        void synchroniseParameterBuffers() noexcept
        {
            for (auto& parameter : m_parameters)
            {
                parameter.second.get().syncWriteBuffer();
            }
        }

        //! Verifies parameters after they are set, to be called after parameters of this component are modified.
        //! The checks need to run on the inactive buffer values.
        virtual std::optional<fgc4::utils::Warning> verifyParameters()
        {
            return {};
        }

      protected:
        std::string const m_component_type;
        std::string const m_name;
        Component*        m_parent{nullptr};

        ParameterList m_parameters;
        ChildrenList  m_children;
    };

    // ************************************************************
    // Constant for denoting that the component constructed is independent (has no parents)
    constexpr Component* independent_component = nullptr;

}   // namespace vslib
