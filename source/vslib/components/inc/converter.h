#pragma once

#include "component.h"
#include "rootComponent.h"

namespace vslib
{
    class IConverter : public Component
    {
      public:
        IConverter(std::string_view name, RootComponent& root)
            : Component("Converter", name, root)
        {
        }

        void virtual init()           = 0;
        void virtual backgroundTask() = 0;
    };
}