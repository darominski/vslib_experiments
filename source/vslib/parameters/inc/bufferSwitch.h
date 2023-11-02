//! @file
//! @brief File defining a global buffer switch for accessing Parameter buffers.
//! @author Dominik Arominski

#pragma once

#include "nonCopyableNonMovable.h"

namespace vslib::parameters
{
    class BufferSwitch : public NonCopyableNonMovable
    {
      public:
        //! Returns the state of the buffer switch
        //!
        //! @return Either 0 or 1, depending on the switch state
        [[nodiscard]] unsigned short static getState() noexcept
        {
            return m_buffer_switch;
        }

        //! Flips the switch state between 0 and 1
        static void flipState() noexcept
        {
            m_buffer_switch ^= 1;
        }

      private:
        BufferSwitch() = default;
        inline static unsigned short m_buffer_switch
            = 0;   // used to define which is the read buffer in use, values: 0 or 1
    };
}   // namespace vslib::parameters
