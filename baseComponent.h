//! @file
//! @brief File containing the common interface of all components to be reused.
//! @author Dominik Arominski

#pragma once

#include <string>

namespace component
{
    class BaseComponent
    {
      public:
        BaseComponent(std::string_view component_name, std::string_view name)
            : m_component_name(component_name),
              m_name(name)
        {
        }

        [[nodiscard]] const auto& getName() const
        {
            return m_name;
        }

      private:
        const std::string m_component_name;   // alternative to enum class type for Param ID building
        const std::string m_name;
    };
}   // namespace component