//! @file
//! @brief Method definitions for the ParameterRegistry class.
//! @author Dominik Arominski

#include <regex>

#include "errorCodes.hpp"
#include "errorMessage.hpp"
#include "fmt/format.h"
#include "parameterRegistry.hpp"

using namespace nlohmann;
using namespace fgc4::utils;

namespace vslib
{
    bool ParameterRegistry::parametersInitialized() const noexcept
    {
        return std::all_of(
            std::cbegin(m_parameters), std::cend(m_parameters),
            [](const auto& parameter)
            {
                return parameter.second.get().isInitialized();
            }
        );
    }

    bool ParameterRegistry::parametersValidated() const noexcept
    {
        return std::all_of(
            std::cbegin(m_parameters), std::cend(m_parameters),
            [](const auto& parameter)
            {
                return parameter.second.get().isValidated();
            }
        );
    }

    void ParameterRegistry::checkNameFormatting(std::string_view parameter_name)
    {
        // Regex to match the expected full name formatting of Components.
        // At the bottom of the hierarchy there is a user-defined Component derived
        // from the RootComponent. It may have an arbitrary user-defined name.
        // The remainder of the hierarchy will be filled by arbitrarily long sequence of snake_case
        // words separated by dots.
        const std::regex re("^([a-z0-9]+(_[a-z0-9]+)*)(\\.([a-z0-9]+(_[a-z0-9]+)*))*$");
        const auto       match = std::regex_match(std::string(parameter_name), re);

        if (!match)
        {
            Error error_message(
                fmt::format(
                    "Parameter name: {} contains characters that are not allowed. Only lower-case letters, numbers, "
                    "and underscores are allowed.\n",
                    parameter_name
                ),
                errorCodes::name_formatting_incorrect
            );
            throw std::runtime_error("Parameter name formatting incorrect!");
        }
    }

    void ParameterRegistry::addToRegistry(std::string_view parameter_name, IParameter& parameter_reference)
    {
        if (m_parameters.find(std::string(parameter_name)) != m_parameters.end())
        {
            Error error_message(
                std::string("Parameter name: ") + std::string(parameter_name)
                    + std::string(" already defined in the registry!\n"),
                errorCodes::name_already_used
            );
            throw std::runtime_error("Parameter name already exists!");
        }
        checkNameFormatting(parameter_name);
        m_parameters.emplace(parameter_name, parameter_reference);
    }
}   // namespace vslib
