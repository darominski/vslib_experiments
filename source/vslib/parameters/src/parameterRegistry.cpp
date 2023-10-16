//! @file
//! @brief Method definitions for the ParameterRegistry class.
//! @author Dominik Arominski

#include <ranges>
#include <type_traits>

#include "errorMessage.h"
#include "fmt/format.h"
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
        if (m_parameters.find(std::string(parameter_name)) != m_parameters.end())
        {
            fgc4::utils::Error error_message(
                std::string("Parameter name: ") + std::string(parameter_name)
                    + std::string(" already defined in the registry!\n"),
                fgc4::utils::constants::error_name_already_used
            );
            std::cerr << fmt::format("{}", error_message);
            throw std::runtime_error("Parameter name already exists!");
        }
        m_parameters.emplace(parameter_name, parameter_reference);
    }
}