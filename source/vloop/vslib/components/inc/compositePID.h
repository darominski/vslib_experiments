//! @file
//! @brief Composite component holding other components but no parameters.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "component.h"
#include "pid.h"

namespace vslib::components
{
    class CompositePID : public Component
    {
      public:
        CompositePID(const std::string& name, Component* parent)
            : Component("CompositePID", name, parent),
              pid1("pid_1", this),
              pid2("pid_2", this)
        {
        }

        PID pid1;
        PID pid2;
    };
}   // namespace components