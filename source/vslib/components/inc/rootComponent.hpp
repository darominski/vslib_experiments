//! @file
//! @brief Definition of the RootComponent, a special class to serve as a base of the Component hierarchy.
//! @author Dominik Arominski

#pragma once

#include "component.hpp"

namespace vslib
{

    using ComponentRef = std::reference_wrapper<Component>;
    using ChildrenList = std::vector<ComponentRef>;

    class RootComponent : public Component
    {

      public:
        //! Creates the RootComponent with type, name as the base of the hierarchy for Components.
        RootComponent() noexcept
            : Component("Root", "root")
        {
        }
    };

}   // namespace vslib
