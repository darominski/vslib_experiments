#pragma once

#include "component.hpp"
#include "rootComponent.hpp"

namespace vslib
{
    class IConverter : public Component
    {
      public:
        //! Interface for the constructor of Converter class, binds construction to the Component class,
        //! allowing the derived user-defined Component to have runtime-settable Parameters.
        //!
        //! @param name Name of this Converter
        //! @param root Parent of this Converter
        IConverter(std::string_view name, RootComponent& root)
            : Component("Converter", name, root)
        {
        }

        //! Method to be filled with initialization logic when the binary is fully configured.
        void virtual init() = 0;

        //! Background task to be executed at each iteration in the spare time, non real-time.
        void virtual backgroundTask() = 0;
    };
}