//! @file
//! @brief Method definitions for the ParameterRegistry class.
//! @author Dominik Arominski

#include <regex>

#include "errorCodes.h"
#include "errorMessage.h"
#include "fmt/format.h"
#include "parameterRegistry.h"

using namespace nlohmann;
using namespace fgc4::utils;

namespace vslib
{
    void ParameterRegistry::checkNameFormatting(const std::string& parameter_name)
    {
        const std::regex re("^([a-z0-9]+(_[a-z0-9]+)*)(.([a-z0-9]+(_[a-z0-9]+)*))*$");
        std::smatch      m;

        const auto match = std::regex_match(parameter_name, m, re);

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
        checkNameFormatting(std::string(parameter_name));
        m_parameters.emplace(parameter_name, parameter_reference);
    }
}   // namespace vslib