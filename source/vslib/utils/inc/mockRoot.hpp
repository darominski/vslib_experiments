#pragma once

#include "../inc/rootComponent.hpp"

namespace vslib
{
    class MockRoot : public RootComponent
    {
      public:
        MockRoot()
            : RootComponent("root")
        {
        }

        void init() override
        {
        }

        void backgroundTask() override
        {
        }
    };
}   // namespace vslib
