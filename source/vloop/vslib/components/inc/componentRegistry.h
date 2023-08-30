//! @file
//! @brief File definining registry with all component references.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <string>

#include "nlohmann/json.hpp"

extern unsigned short buffer_switch;

namespace vslib::component
{
    class BaseComponent;   // forward declaration

    class ComponentRegistry
    {
      public:
        // the registry shall not be assignable, nor clonable, nor moveable
        ComponentRegistry(ComponentRegistry& other)  = delete;
        ComponentRegistry(ComponentRegistry&& other) = delete;
        void operator=(const ComponentRegistry&)     = delete;
        void operator=(const ComponentRegistry&&)    = delete;
        ~ComponentRegistry() = default;   // will never be called, lifetime equal to that of the program

        static ComponentRegistry& instance()
        {
            // Registry is constructed on first access
            static ComponentRegistry m_instance;
            return m_instance;
        }

        auto const& getComponents() const
        {
            return m_components;
        }

        void addToRegistry(std::string_view, BaseComponent&);

        nlohmann::json createManifest() const;

      private:
        ComponentRegistry() = default;
        std::map<std::string, std::reference_wrapper<BaseComponent>> m_components;
    };
}   // namespace parameters