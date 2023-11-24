//! @file
//! @brief File containing a thin-layer interface to configure a peripheral interrupt.
//! @author Dominik Arominski

#pragma once

#include "interrupt.h"

namespace vslib
{
    class PeripheralInterrupt : public Interrupt
    {
      public:
        //! Constructor for PeripheralInterrupt
        //!
        //! @param handler_function Function to be called when the interrupt triggers
        //! @param interrupt_id Platform-dependent interrupt ID
        //! @param priority Priority level of the interrupt
        PeripheralInterrupt(
            std::function<void(void)> handler_function, int interrupt_id, bmboot::PayloadInterruptPriority priority
        )
            : Interrupt(std::move(handler_function)),
              m_interrupt_id{interrupt_id},
              m_priority{priority}
        {
            bmboot::setupInterruptHandling(m_interrupt_id, m_priority, m_interrupt_handler);
        }

        //! Starts peripheral interrupt
        void start() override
        {
            bmboot::enableInterruptHandling(m_interrupt_id);
        }

        //! Stops the peripheral interrupt from triggering
        void stop() override
        {
            bmboot::disableInterruptHandling(m_interrupt_id);
        }

      private:
        int                              m_interrupt_id;
        bmboot::PayloadInterruptPriority m_priority;
    };
}   // namespace vslib