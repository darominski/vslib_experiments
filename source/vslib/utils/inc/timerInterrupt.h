//! @file
//! @brief File containing a thin-layer interface to configure a periodic timer interrupt.
//! @author Dominik Arominski

#pragma once

#include "interrupt.h"

namespace vslib
{
    class TimerInterrupt : public Interrupt
    {
      public:
        TimerInterrupt(std::function<void(void)> handler_function, int microsecond_delay)
            : Interrupt(std::move(handler_function)),
              m_microsecond_delay{microsecond_delay}
        {
            assert((microsecond_delay > 0) && "Delay for the timing interrupt must be a positive number.");
        }

        void start() override
        {
            bmboot::startPeriodicInterrupt(m_microsecond_delay, m_interrupt_handler);
        }

        void stop() override
        {
            bmboot::stopPeriodicInterrupt();
        }

      private:
        int m_microsecond_delay;
    };
}   // namespace vslib