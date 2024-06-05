//! @file
//! @brief Component holding state enumeration.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <string>

#include "component.h"

namespace vslib
{
    enum class States
    {
        unconfigured,
        ready,
        off,
        fault
    };

    template<uint32_t StatesLength>
    class State : public Component
    {
      public:
        State(std::string_view name, Component* parent)
            : Component("State", name, parent),
              states(*this, "states")
        {
        }

        Parameter<std::array<States, StatesLength>> states;
    };
}   // namespace vslib