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
        BaseComponent(const std::string& name)
            : m_name(name)
        {
        }

        [[nodiscard]] const std::string& getName() const
        {
            return m_name;
        }

      private:
        std::string m_name;
    };
}   // namespace component