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
              m_name(name)
        {
        }

        virtual ~IComponent() = default;

        //! Adds a child RootComponent to this RootComponent.
        //!
        //! @param child Child Component to be added to this RootComponent
        void addChild(IComponent& child) noexcept
        {
            m_children.emplace_back(child);
        }

        //! Serializes this Component to JSON, including all children Components and Parameters
        //! across the entire hierarchy.
        //!
        //! @return Returns a fully-serialized Component as a JSON object
        virtual StaticJson serialize() const noexcept = 0;

        // ************************************************************
        // Getters

        //! Provides the name of this RootComponent.
        //!
        //! @return String_view of the component name
        [[nodiscard]] std::string_view getName() const noexcept
        {
            return m_name;
        }

        //! Provides the full name of this IComponent, including its type, to be overriden by derived classes.
        //!
        //! @return String_view of the component name
        virtual std::string_view getFullName() const noexcept = 0;

        //! Provides the container with all children belonging to this RootComponent.
        //!
        //! @return Vector with references to all children of this RootComponent
        [[nodiscard]] auto const& getChildren() const noexcept
        {
            return m_children;
        }

        //! Provides the map with all names and references to all parameters registered to this Component.
        //!
        //! @return Map with names and references to all parameters of this Component
        [[nodiscard]] auto const& getParameters() const noexcept
        {
            return m_parameters;
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

        virtual std::optional<fgc4::utils::Warning> verifyParameters()                     = 0;
        virtual void                                flipBufferState() noexcept             = 0;
        virtual void                                synchroniseParameterBuffers() noexcept = 0;

      protected:
        std::string m_component_type;   //!< Type of this RootComponent
        std::string m_name;             //!< Name of this RootComponent

        ChildrenList  m_children;     //!< Container with all children Components registered to this Component
        ParameterList m_parameters;   //!< Container with all Parameters registered to this Component
    };

}   // namespace vslib
