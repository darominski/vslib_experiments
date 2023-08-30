//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "baseComponent.h"
#include "parameter.h"

namespace vslib::component
{
    class PID : public BaseComponent
    {
      public:
        PID()                      = delete;   // anonymous PIDs are forbidden
        PID(PID& other)            = delete;   // cloning objects is forbidden
        PID& operator=(const PID&) = delete;   // copy-assignment is forbidden

        PID(const std::string& name, double _p = 0.0, double _i = 0.0, double _d = 0.0)
            : BaseComponent(constants::component_type_pid, name),
              p(*this, "p", _p, -10.0, 10.0),
              i(*this, "i", _i, -10.0, 10.0),
              d(*this, "d", _d)   // default limits apply here
        {
        }

        parameters::Parameter<double> p;
        parameters::Parameter<double> i;
        parameters::Parameter<double> d;
    };
}   // namespace component