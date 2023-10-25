//! @file
//! @brief File containing a thin-layer interface to configure interrupts in VSlib.
//! @author Dominik Arominski

#pragma once

#include <cassert>
#include <functional>

#include "bmboot/payload_runtime.hpp"

namespace vslib
{

    class Interrupt
    {
      public:
        //! Constructor for the Interrupt object
        Interrupt(std::function<void(void)> handler_function)
        {
#ifdef PERFORMANCE_TESTS
            m_interrupt_handler = [this, handler_function]()
            {
                preConditions();
                handler_function();
                postConditions(0);   // and this value needs to be sent somewhere
            };
#else
            m_interrupt_handler = handler_function;
#endif
        }

        virtual ~Interrupt() = default;

        //! Starts the interrupt
        virtual void start() = 0;

        //! Stops the interrupt
        virtual void stop() = 0;

      protected:
        std::function<void(void)> m_interrupt_handler;

#ifdef PERFORMANCE_TESTS
        //! Defines the preconditions necessary to estimate the execution time of the interrupt handler
        int64_t preConditions()
        {
            // implementation of polling the CPU clock is required
            return 0;
        }

        //! Defines the postconditions necessary to estimate the execution time of the interrupt handler
        int64_t postConditions(int64_t)
        {
            // implementation of polling the CPU clock is required
            return 0;
        }
#endif
    };

    class TimerInterrupt : public Interrupt
    {
      public:
        TimerInterrupt(std::function<void(void)> handler_function, int microsecond_delay)
            : Interrupt(handler_function),
              m_microsecond_delay{microsecond_delay}
        {
            assert((microseconds_delay > 0) && "Delay for the timing interrupt must be a positive number.");
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