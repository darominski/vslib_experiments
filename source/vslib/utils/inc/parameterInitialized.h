#include <algorithm>

#include "parameterRegistry.h"

#pragma once

namespace vslib::utils
{
    bool parametersInitialized()
    {
        const auto& parameters = ParameterRegistry::instance().getParameters();
        return std::all_of(
            std::cbegin(parameters), std::cend(parameters),
            [](const auto& parameter)
            {
                bool const initialized = parameter.second.get().isInitialized();
                if (!initialized)
                {
                    std::cerr << parameter.first << std::endl;
                }
                return initialized;
            }
        );
    }
}   // namespace vslib::utils