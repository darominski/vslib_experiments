//! @file
//! @brief Class defining version objects to be used throughout FGC4 codebase.
//! @author Dominik Arominski

#pragma once

namespace fgc4::utils
{
    class Version
    {
      public:
        constexpr Version(short _major, short _minor, short _revision)
            : major(_major),
              minor(_minor),
              revision(_revision)
        {
        }

        short major;
        short minor;
        short revision;
    };
}   // namespace fgc4::utils