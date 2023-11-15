//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib
{
    class PID : public Component
    {
      public:
        PID(std::string_view name, Component* parent = nullptr)
            : Component("PID", name, parent),
              p(*this, "p", -10.0, 10.0),   // min limit: -10, max limit: 10
              i(*this, "i", -10.0, 10.0),
              d(*this, "d")   // default limits apply here
        {
        }

        Parameter<double> p;
        Parameter<double> i;
        Parameter<double> d;
    };
}   // namespace vslib
