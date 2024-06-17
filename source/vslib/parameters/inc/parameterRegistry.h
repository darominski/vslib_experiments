//! @file
//! @brief File definining registry with references to all settable parameters.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <string>

#include "iparameter.h"
#include "json/json.hpp"
#include "nonCopyableNonMovable.h"

namespace vslib
{
    class ParameterRegistry : public NonCopyableNonMovable
    {
        using ParameterReference = std::reference_wrapper<IParameter>;

      public:
        //! Provides an instance of the singleton registry
        //!
        //! @return Singular instance of the parameter registry
        static ParameterRegistry& instance()
        {
            // Registry is constructed on first access
            static ParameterRegistry m_instance;
            return m_instance;
        }

        //! Provides map of all created parameter names and references to them.
        //!
        //! @return Map with full parameter names and their references
        [[nodiscard]] auto const& getParameters() const
        {
            return m_parameters;
        }

        //! Adds a new entry to the Parameter registry.
        //!
        //! @param parameter_name Name of the parameter to be added to the parameter registry
        //! @param parameter_reference Reference to the parameter being added to the parameter registry
        void addToRegistry(std::string_view parameter_name, IParameter& parameter_reference);

        //! Clears the registry.
        void clearRegistry() noexcept
        {
            m_parameters.clear();
        }

      private:
        ParameterRegistry() = default;                            //!< Default constructor
        std::map<std::string, ParameterReference> m_parameters;   //!< Map holding references to all Parameters
    };
}   // namespace vslib
