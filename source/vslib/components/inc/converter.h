#pragma once

#include "component.h"

namespace vslib
{
    class IConverter : public Component
    {
      public:
        IConverter(std::string_view name, Component* root)
            : Component("Converter", name, root)
        {
        }

        void virtual init()           = 0;
        void virtual backgroundTask() = 0;
    };
}