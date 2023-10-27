//! @file
//! @brief File definining registry with all component references.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <string>

#include "nonCopyableNonMovable.h"
#include "staticJson.h"

namespace vslib::components
{
    class Component;   // forward declaration

    class ComponentRegistry : public NonCopyableNonMovable
    {
        using ComponentReference = std::reference_wrapper<Component>;

      public:
        //! Provides an instance of the singleton registry
        //!
        //! @return Singular instance of the component registry
        static ComponentRegistry& instance()
        {
            // Registry is constructed on first access
            static ComponentRegistry m_instance;
            return m_instance;
        }

        //! Provides map of all created component names and references to them
        //!
        //! @return Map with component names and their references
        auto const& getComponents() const
        {
            return m_components;
        }

        void addToRegistry(std::string_view, Component&);

        void clearRegistry() noexcept
        {
            m_components.clear();
        }

        fgc4::utils::StaticJson createParameterMap() const;

      private:
        ComponentRegistry() = default;
        std::map<std::string, ComponentReference> m_components;   // holds all independent components
    };
}   // namespace vslib::components
