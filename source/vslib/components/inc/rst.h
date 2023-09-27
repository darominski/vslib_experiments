#pragma once

#include <array>
#include <string>

#include "component.h"
#include "magic_enum/magic_enum.hpp"
#include "parameter.h"

namespace vslib::components
{
    // Helper enumeration class for definition of all relevant statuses
    enum class Status
    {
        uninitialized,
        ready,
        updating,
        fault
    };

    // ************************************************************

    class RST : public Component
    {
      public:
        RST(std::string_view name, Component* parent, std::array<double, 4> _r, bool _flag)
            : Component(constants::component_type_rst, name, parent),
              r(*this, "r", _r, -12.0, 10.0),
              status(*this, "status", Status::uninitialized),
              flag(*this, "flag", _flag)
        {
        }

        //! Provides status value as a string
        //!
        //! @return String view of the status of this component
        [[nodiscard]] std::string_view getStatusAsStr() const
        {
            return magic_enum::enum_name(status.value());
        }

        parameters::Parameter<std::array<double, 4>> r;
        parameters::Parameter<Status>                status;
        parameters::Parameter<bool>                  flag;
    };
}   // namespace components
