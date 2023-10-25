//! @file
//! @brief File definining registry with all component references.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <string>

#include "json/json.hpp"
#include "nonCopyableNonMovable.h"

extern unsigned short buffer_switch;

namespace vslib::components
{
    class Component;   // forward declaration

    class ComponentRegistry : public NonCopyableNonMovable
    {
      public:
        ~ComponentRegistry() override   // is not expected to be called, lifetime equal to that of the application
        {
            m_components.clear();
        }

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

        nlohmann::json createManifest() const;

      private:
        ComponentRegistry() = default;
        std::map<std::string, std::reference_wrapper<Component>> m_components;   // holds all independent components
    };
}   // namespace components
