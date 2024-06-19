#include <vector>

#include "component.h"

#pragma once

namespace vslib::utils
{
    bool validateComponent(const std::vector<std::reference_wrapper<Component>>& children);
    bool validateComponents(Component& root);

    bool validateComponents(Component& root)
    {
        // validate the root Component first:
        const auto& root_warning = root.verifyParameters();
        if (!root_warning.has_value())
        {
            root.flipBufferState();
        }
        else
        {
            return false;
        }

        // validate all children and their children tree indefinitely deeply
        return validateComponent(root.getChildren());
    }

    bool validateComponent(const std::vector<std::reference_wrapper<Component>>& children)
    {
        bool success = true;
        for (const auto& child : children)
        {
            auto& component = child.get();
            if (component.parametersInitialized())
            {
                const auto& warning = component.verifyParameters();
                success             = !warning.has_value();
                if (success)
                {
                    component.flipBufferState();
                }
                else
                {
                    return false;
                }
                // if there is an issue: it is logged, the component's buffer is not flipped
                component.synchroniseParameterBuffers();
            }
            success = validateComponent(component.getChildren());
            if (!success)
            {
                return success;
            }
        }
        return success;
    }

}   // vslib::utils