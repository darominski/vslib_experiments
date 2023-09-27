//! @file
//! @brief File definining registry with references to all settable parameters.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <string>

#include "iparameter.h"
#include "nlohmann/json.hpp"

extern unsigned short buffer_switch;   // used to define which is the read buffer in use, values: 0 or 1

namespace vslib::parameters
{
    class ParameterRegistry
    {
      public:
        // the registry shall not be assignable nor clonable
        ParameterRegistry(ParameterRegistry& other)  = delete;
        ParameterRegistry(ParameterRegistry&& other) = delete;
        void operator=(const ParameterRegistry&)     = delete;
        void operator=(const ParameterRegistry&&)    = delete;
        ~ParameterRegistry() = default;   // will never be called, lifetime equal to that of the program

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
