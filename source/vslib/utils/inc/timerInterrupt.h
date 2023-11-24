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
        //! Constructor for TimerInterrupt
        //!
        //! @param handler_function Function to be called when an interrupt triggers
        //! @param microsecond_delay Delay between interrupts in integer increments of a microsecond
        TimerInterrupt(std::function<void(void)> handler_function, int microsecond_delay)
            : Interrupt(std::move(handler_function)),
              m_microsecond_delay{microsecond_delay}
        {
            assert((microsecond_delay > 0) && "Delay for the timing interrupt must be a positive number.");
            bmboot::setupPeriodicInterrupt(m_microsecond_delay, m_interrupt_handler);
        }

        //! Starts periodic interrupt
        void start() override
        {
            bmboot::startPeriodicInterrupt();
        }

        //! Stops periodic interrupt from triggering
        void stop() override
        {
            bmboot::stopPeriodicInterrupt();
        }

      private:
        int m_microsecond_delay;
    };
}   // namespace vslib