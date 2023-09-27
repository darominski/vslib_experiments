//! @file
//! @brief Method definitions for the ParameterRegistry class.
//! @author Dominik Arominski

#include <ranges>
#include <type_traits>

#include "parameterRegistry.h"

using json = nlohmann::json;

namespace vslib::parameters
{
    //! Adds a new entry to the parameter registry
    //!
    //! @param parameter_name Name of the parameter to be added to the parameter registry
    //! @param parameter_reference Reference to the parameter being added to the parameter registry
    void ParameterRegistry::addToRegistry(std::string_view parameter_name, IParameter& parameter_reference)
    {
        m_parameters.emplace(parameter_name, parameter_reference);
    }
}