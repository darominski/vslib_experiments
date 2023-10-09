//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "parameter.h"

namespace vslib::components
{
    class PID : public Component
    {
      public:
        PID(std::string_view name, Component* parent = nullptr, double _p = 0.0, double _i = 0.0, double _d = 0.0)
            : Component("PID", name, parent),
              p(*this, "p", _p, -10.0, 10.0),
              i(*this, "i", _i, -10.0, 10.0),
              d(*this, "d", _d)   // default limits apply here
        {
        }

        parameters::Parameter<double> p;
        parameters::Parameter<double> i;
        parameters::Parameter<double> d;
    };
}   // namespace components
