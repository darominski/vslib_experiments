//! @file
//! @brief File containing versions for all used communication interfaces.
//! @author Dominik Arominski

#pragma once

#include <string>

namespace vslib::version
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

    constexpr Version json_command(0, 1);
    constexpr Version json_parameter_map(0, 1);
}
