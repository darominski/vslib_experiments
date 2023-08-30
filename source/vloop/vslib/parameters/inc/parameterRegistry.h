//! @file
//! @brief File definining registry with references to all settable parameters.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <string>
#include <tuple>

#include "iparameter.h"
#include "nlohmann/json.hpp"

extern unsigned short buffer_switch;

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

        static ParameterRegistry& instance()
        {
            // Registry is constructed on first access
            static ParameterRegistry m_instance;
            return m_instance;
        }

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