//! @file
//! @brief File defining a base non-copyable and non-movable class to be reused by the library.
//! @author Dominik Arominski

#pragma once

namespace vslib
{
    class NonCopyableNonMovable
    {
      public:
        NonCopyableNonMovable()                                        = default;
        virtual ~NonCopyableNonMovable()                               = default;
        // Disable copy and move
        NonCopyableNonMovable(const NonCopyableNonMovable&)            = delete;
        NonCopyableNonMovable(NonCopyableNonMovable&&)                 = delete;
        NonCopyableNonMovable& operator=(const NonCopyableNonMovable&) = delete;
        NonCopyableNonMovable& operator=(NonCopyableNonMovable&&)      = delete;
    };
}