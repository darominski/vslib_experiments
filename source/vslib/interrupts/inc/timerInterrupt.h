//! @file
//! @brief File containing a thin-layer interface to configure a periodic timer interrupt.
//! @author Dominik Arominski

#pragma once

#include "bmboot.hpp"
#include "interrupt.h"
#include "parameter.h"

namespace vslib
{
    template<class Converter>
    class TimerInterrupt : public Interrupt<Converter>
    {
      public:
        //! Constructor for TimerInterrupt.
        //!
        //! @param handler_function Function to be called when an interrupt triggers
        TimerInterrupt(
            std::string_view name, Converter& converter, int64_t delay,
            std::function<void(Converter&)> handler_function =
                [](Converter&)
            {
                ;
            }
        )
            : Interrupt<Converter>(name, converter, handler_function),
              m_delay(std::chrono::microseconds(delay))
        {
            static_assert(
                std::derived_from<Converter, IConverter>,
                "The interrupt's template class must be derived from IConverter."
            );
            bmboot::setupPeriodicInterrupt(m_delay, this->m_interrupt_handler);
        }

        //! Starts periodic interrupt.
        void start() override
        {
            bmboot::startPeriodicInterrupt();
        }

        //! Stops periodic interrupt from triggering.
        void stop() override
        {
            bmboot::stopPeriodicInterrupt();
        }

        //! Sets the delay of the timer interrupt.
        //!
        //! @param delay Interrupt delay in microseconds
        void setDelay(int64_t delay) noexcept
        {
            m_delay = std::chrono::microseconds(delay);
            bmboot::setupPeriodicInterrupt(m_delay, this->m_interrupt_handler);
        }

        //! Returns the delay in microseconds.
        int64_t getDelay() const
        {
            return m_delay.count();
        }

      private:
        std::chrono::microseconds m_delay;   //!< delay in microseconds
    };
}   // namespace vslib
