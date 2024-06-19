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
                return parameter.second.get().isInitialized();
            }
        );
    }
}   // namespace vslib::utils