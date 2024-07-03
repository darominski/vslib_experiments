#pragma once

#include "component.h"

namespace user
{

    class IConverter : public vslib::Component
    {
      public:
        IConverter(std::string_view name, Component* root)
            : vslib::Component("Converter", name, root)
        {
        }

        void virtual init()           = 0;
        void virtual backgroundTask() = 0;
    };
}