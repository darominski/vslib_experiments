//! @file
//! @brief Defines class for a factory of all supported filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "boxFilter.h"
#include "component.h"
#include "firFilter.h"
#include "iirFilter.h"
#include "parameter.h"

namespace vslib
{
    enum class FilterType
    {
        Box,
        FIR,
        IIR
    };

    class FilterFactory
    {
      public:
        template<size_t CoefficientsLength, double maximalFilteredValue = 1e5>
        static std::unique_ptr<Filter> createFilter(FilterType type, std::string_view name, Component* parent = nullptr)
        {
            static_assert(CoefficientsLength > 1, "Coefficient length must be a positive number larger than one.");
            switch (type)
            {
                case FilterType::Box:
                {
                    return std::make_unique<BoxFilter<CoefficientsLength, maximalFilteredValue>>(name, parent);
                    break;
                }
                case FilterType::FIR:
                {
                    return std::make_unique<FIRFilter<CoefficientsLength>>(name, parent);
                    break;
                }
                case FilterType::IIR:
                {
                    return std::make_unique<IIRFilter<CoefficientsLength>>(name, parent);
                    break;
                }
            }
        }
    };
}