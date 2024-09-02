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
    class IComponent : public NonCopyableNonMovable
    {

      public:
        using ParameterReference = std::reference_wrapper<IParameter>;
        using ParameterList      = std::vector<std::pair<std::string, ParameterReference>>;
        using ComponentReference = std::reference_wrapper<IComponent>;
        using ChildrenList       = std::vector<ComponentReference>;
        using StaticJson         = fgc4::utils::StaticJson;

        IComponent(std::string_view type, std::string_view name)
            : m_component_type(type),
              m_name(name),
              m_full_name(name)
        {
        }

        virtual ~IComponent() = default;

        //! Adds a child component to this component.
        //!
        //! @param child Child Component to be added to this component
        void addChild(IComponent& child) noexcept
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


        // ************************************************************
        // Virtual methods

        //! Serializes this Component to JSON, including all children component and Parameters
        //! across the entire hierarchy.
        //!
        //! @return Returns a fully-serialized Component as a JSON object
        virtual StaticJson serialize() const noexcept = 0;

        //! Verifies parameters after they are set, to be called after parameters of this component are modified.
        //! The checks need to run on the inactive buffer values.
        virtual std::optional<fgc4::utils::Warning> verifyParameters() = 0;

      protected:
        std::string m_component_type;   //!< Type of this RootComponent
        std::string m_name;             //!< Name of this RootComponent
        std::string m_full_name;        //!< Full name of this component, including hierarchy

        ChildrenList  m_children;     //!< Container with all children component registered to this component
        ParameterList m_parameters;   //!< Container with all Parameters registered to this component
    };

}   // namespace vslib
