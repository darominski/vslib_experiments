//! @file
//! @brief Definition of the RootComponent, a special class to serve as a base of the Component hierarchy.
//! @author Dominik Arominski

#pragma once

#include <vector>

#include "component.hpp"

namespace vslib
{
    using ComponentRef = std::reference_wrapper<Component>;
    using ChildrenList = std::vector<ComponentRef>;

    class RootComponent : public Component
    {
      public:
        //! Creates the RootComponent with type, name as the base of the hierarchy for Components.
        RootComponent(std::string_view name) noexcept
            : Component("Root", name)
        {
        }

        virtual ~RootComponent() = default;

        //! Method to be filled with initialization logic when the binary is fully configured.
        void virtual init() = 0;

        //! Background task to be executed at each iteration in the spare time, non real-time.
        void virtual backgroundTask() = 0;
    };
}   // namespace vslib
