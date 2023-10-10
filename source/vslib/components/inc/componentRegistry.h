//! @file
//! @brief File definining registry with all component references.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <string>

#include "json/json.hpp"

extern unsigned short buffer_switch;

namespace vslib::components
{
    class Component;   // forward declaration

    class ComponentRegistry
    {
      public:
        // the registry shall not be assignable, nor clonable, nor moveable
        ComponentRegistry(ComponentRegistry& other)  = delete;
        ComponentRegistry(ComponentRegistry&& other) = delete;
        void operator=(const ComponentRegistry&)     = delete;
        void operator=(const ComponentRegistry&&)    = delete;
        ~ComponentRegistry() = default;   // will never be called, lifetime equal to that of the program

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
        std::map<std::string, std::reference_wrapper<Component>> m_components;
    };
}   // namespace components
