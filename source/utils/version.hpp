//! @file
//! @brief Class defining version objects to be used throughout FGC4 codebase.
//! @author Dominik Arominski

#pragma once

namespace fgc4::utils
{
    class Version
    {
      public:
        constexpr Version(int32_t _major, int32_t _minor, int32_t _revision)
            : major(_major),
              minor(_minor),
              revision(_revision)
        {
        }

        int32_t major;
        int32_t minor;
        int32_t revision;
    };
}   // namespace fgc4::utils