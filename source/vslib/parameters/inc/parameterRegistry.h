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

namespace vslib::parameters
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

        //! Provides map of all created parameter names and references to them
        //!
        //! @return Map with full parameter names and their references
        [[nodiscard("Parameters should not be discarded.")]] auto const& getParameters() const
        {
            return m_parameters;
        }

        void addToRegistry(std::string_view, IParameter&);

        void clearRegistry() noexcept
        {
            m_parameters.clear();
        }

      private:
        ParameterRegistry() = default;
        std::map<std::string, ParameterReference> m_parameters;
    };
}   // namespace parameters
