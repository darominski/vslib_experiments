//! @file
//! @brief File containing versions for all used communication interfaces.
//! @author Dominik Arominski

#pragma once

#include <string>

namespace vslib::utils::version
{
    class Version
    {
      public:
        constexpr Version(short _major, short _minor)
            : major(_major),
              minor(_minor)
        {
        }

        short major;
        short minor;
    };

    // ************************************************************

    constexpr std::string_view json_command       = "0.1";
    constexpr std::string_view json_parameter_map = "0.1";
}
