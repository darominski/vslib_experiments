//! @file
//! @brief Method definitions for the ParameterRegistry class.
//! @author Dominik Arominski

#include "errorCodes.h"
#include "errorMessage.h"
#include "fmt/format.h"
#include "parameterRegistry.h"

using namespace nlohmann;
using namespace fgc4::utils;

namespace vslib
{
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
        m_parameters.emplace(parameter_name, parameter_reference);
    }
}   // namespace vslib