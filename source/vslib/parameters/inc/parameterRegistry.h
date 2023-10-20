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

namespace vslib::parameters
{
    class ParameterRegistry
    {
      public:
        // The registry is a singleton, thus it is meant to be copied, moved, or assigned to in any way
        ParameterRegistry(ParameterRegistry&)     = delete;
        ParameterRegistry(ParameterRegistry&&)    = delete;
        void operator=(const ParameterRegistry&)  = delete;
        void operator=(const ParameterRegistry&&) = delete;
        ~ParameterRegistry()   // is not expected to be called, lifetime equal to that of the program
        {
            m_parameters.clear();
        }
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

      private:
        ParameterRegistry() = default;
        std::map<std::string, std::reference_wrapper<IParameter>> m_parameters;
    };
}   // namespace parameters
