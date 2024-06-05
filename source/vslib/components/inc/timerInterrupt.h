//! @file
//! @brief File containing a thin-layer interface to configure a periodic timer interrupt.
//! @author Dominik Arominski

#pragma once

#include "interrupt.h"
#include "parameter.h"

namespace vslib
{
    class TimerInterrupt : public Interrupt
    {
      public:
        //! Constructor for TimerInterrupt
        //!
        //! @param handler_function Function to be called when an interrupt triggers
        //! @param microsecond_delay Delay between interrupts in integer increments of a microsecond
        TimerInterrupt(
            std::string_view name, Component* parent,
            std::function<void(void)> handler_function =
                []()
            {
                ;
            }
        )
            : Interrupt("TimerInterrupt", name, parent, std::move(handler_function)),
              delay(*this, "delay", 0.0)
        {
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

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            bmboot::setupPeriodicInterrupt(std::chrono::microseconds(delay.value()), m_interrupt_handler);

            return {};
        }

        Parameter<int64_t> delay;
    };
}   // namespace vslib
