//! @file
//! @brief File containing the common interface of all components.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "iparameter.h"
#include "nonCopyableNonMovable.h"
#include "parameterRegistry.h"
#include "parameterSerializer.h"
#include "staticJson.h"

namespace vslib
{

    class Component : public NonCopyableNonMovable
    {
        friend class RootComponent;

      public:
        using ComponentRef  = std::reference_wrapper<Component>;
        using ChildrenList  = std::vector<ComponentRef>;
        using ParameterRef  = std::reference_wrapper<IParameter>;
        using ParameterList = std::vector<std::pair<std::string, ParameterRef>>;
        using StaticJson    = fgc4::utils::StaticJson;

        //! Creates the Component object with the provided type, name, and inside the hierarchy specified by parent.
        //!
        //! @param component_type Type of the Component
        //! @param name Name of the Component, needs to be unique in the type
        //! @param parent Possible parent of this Component, for the root Component should be nullptr
        Component(std::string_view component_type, std::string_view name, Component& parent) noexcept
            : m_component_type(component_type),
              m_name(name),
              m_full_name(name)
        {
            m_full_name = std::string(parent.getFullName()) + "." + std::string(m_name);
            parent.addChild(*this);
        }

        //! Adds a child component to this component.
        //!
        //! @param child Child Component to be added to this component
        void addChild(Component& child) noexcept
        {
            m_children.emplace_back(child);
        }

        // ************************************************************
        // Getters

        //! Provides the name of this component.
        //!
        //! @return String_view of the component name
        [[nodiscard]] std::string_view getName() const noexcept
        {
            return m_name;
        }

        //! Provides the full name of this component.
        //!
        //! @return String_view of the component name
        [[nodiscard]] std::string_view getFullName() const noexcept
        {
            return m_full_name;
        }

        //! Provides the container with all children belonging to this component.
        //!
        //! @return Vector with references to all children of this component
        [[nodiscard]] auto const& getChildren() const noexcept
        {
            return m_children;
        }

        //! Provides the map with all names and references to all parameters registered to this component.
        //!
        //! @return Map with names and references to all parameters of this component
        [[nodiscard]] auto const& getParameters() const noexcept
        {
            return m_parameters;
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
        [[nodiscard]] StaticJson serialize() const noexcept
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
                serialized_parameters.emplace_back(serializer.serialize(parameter.second.get()));
            }

            serialized_component
                = {{"name", m_name},
                   {"type", m_component_type},
                   {"parameters", serialized_parameters},
                   {"components", serialized_children}};
            return serialized_component;
        }

        // ************************************************************
        // Methods for interacting with owned Parameters

        //! Flips the buffer state of all Parameters registered with this component.
        void flipBufferState() noexcept
        {
            for (auto& parameter : m_parameters)
            {
                parameter.second.get().swapBuffers();
            }
        }

        //! Synchronises buffers for all Parameters registered with this component.
        void synchroniseParameterBuffers() noexcept
        {
            for (auto& parameter : m_parameters)
            {
                parameter.second.get().syncWriteBuffer();
            }
        }

        //! Returns the value of the flag informing whether the parameters of this component have been initialized
        //!
        //! @return True if all Parameters of this Component have been initialized, false otherwise
        [[nodiscard]] bool parametersInitialized() const noexcept
        {
            return std::all_of(
                std::cbegin(m_parameters), std::cend(m_parameters),
                [](const auto& parameter)
                {
                    return parameter.second.get().isInitialized();
                }
            );
        }

        //! Sets the validation flag of owned Parameters to true when the validation has been successfully.
        void setParametersValidated() noexcept
        {
            std::for_each(
                getParameters().begin(), getParameters().end(),
                [](auto parameter)
                {
                    parameter.second.get().setValidated(true);
                }
            );
        }

        //! Sets either the validation or the initialization flag of owned Parameters to false
        //! when the validation on the Component level has failed. If they were previously not
        //! successfully validated, the initialization flag is set to false. This is especially
        //! relevant during startup.
        void revokeValidation() noexcept
        {
            std::for_each(
                getParameters().begin(), getParameters().end(),
                [](auto parameter)
                {
                    if (!parameter.second.get().isValidated())
                    {
                        // if Parameter has not been previously validated, the currently set Parameters
                        // are not correct and the entire initialization process needs revoking
                        parameter.second.get().setInitialized(false);
                    }
                    else
                    {
                        parameter.second.get().setValidated(false);
                    }
                }
            );
        }

        //! Verifies parameters after they are set, to be called after parameters of this component are modified.
        //! The checks need to run on the inactive buffer values.
        virtual std::optional<fgc4::utils::Warning> verifyParameters()
        {
            return {};
        }

      protected:
        std::string m_component_type;   //!< Type of this Component
        std::string m_name;             //!< Name of this Component
        std::string m_full_name;        //!< Full name of this component, including hierarchy

        ChildrenList  m_children;     //!< Container with all children component registered to this component
        ParameterList m_parameters;   //!< Container with all Parameters registered to this component

      private:
        //! Creates the Component object with the provided type, name. Intended for creating the root Component.
        //!
        //! @param component_type Type of the Component
        //! @param name Name of the Component, needs to be unique in the type
        //! @param parent Possible parent of this Component, for the root Component should be nullptr
        Component(std::string_view component_type, std::string_view name) noexcept
            : m_component_type(component_type),
              m_name(name),
              m_full_name(name)
        {
        }
    };

}   // namespace vslib
