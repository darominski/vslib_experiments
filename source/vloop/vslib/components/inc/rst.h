#pragma once

#include <array>
#include <string>

#include "baseComponent.h"
#include "magic_enum/magic_enum.hpp"
#include "parameter.h"

namespace vslib::component
{
    enum class Status
    {
        uninitialized,
        ready,
        updating,
        fault
    };

    class RST : public BaseComponent
    {
      public:
        RST()                      = delete;   // disallow users from creating anonymous RSTs
        RST(RST& other)            = delete;   // and cloning objects
        void operator=(const RST&) = delete;   // as well as assigning

        RST(const std::string& name, std::array<double, 4> _r, bool _flag)
            : BaseComponent(constants::component_type_rst, name),
              r(*this, "r", _r, -12.0, 10.0),
              status(*this, "status", Status::uninitialized),
              flag(*this, "flag", _flag)
        {
        }

        [[nodiscard]] std::string_view getStatusAsStr() const
        {
            return magic_enum::enum_name(status.value());
        }

        parameters::Parameter<std::array<double, 4>> r;
        parameters::Parameter<Status>                status;
        parameters::Parameter<bool>                  flag;
    };
}   // namespace component