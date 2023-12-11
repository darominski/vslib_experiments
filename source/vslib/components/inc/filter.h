//! @file
//! @brief Defines the base class for filters.
//! @author Dominik Arominski

#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>

#include "component.h"

namespace vslib
{
    class Filter : public Component
    {
      public:
        Filter(std::string_view type, std::string_view name, Component* parent = nullptr)
            : Component(type, name, parent)
        {
        }

        virtual double filter(double) = 0;
    };
}   // namespace vslib