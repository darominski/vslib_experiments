//! @file
//! @brief File containing a thin-layer interface to configure a periodic timer interrupt.
//! @author Dominik Arominski

#pragma once

#include "bmboot.hpp"
#include "interrupt.hpp"
#include "parameter.hpp"

namespace vslib
{
    template<class Converter>
    class TimerInterrupt : public Interrupt<Converter>
    {
      public:
        //! Constructor for TimerInterrupt.
        //!
        //! @param name Name identifier for this interrupt
        //! @param converter Reference to the parent converter object
        //! @param handler_function Function to be called when an interrupt triggers
        //! @param delay Delay between interrupts in integer increments of a microsecond
        TimerInterrupt(
            std::string_view name, Converter& converter, std::chrono::microseconds delay,
            std::function<void(Converter&)> handler_function =
                [](Converter&)
            {
                ;
            }
        )
            : Interrupt<Converter>(name, converter, handler_function),
              m_delay(delay)
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
        void setDelay(std::chrono::microseconds delay) noexcept
        {
            m_delay = delay;
            bmboot::setupPeriodicInterrupt(m_delay, this->m_interrupt_handler);
        }

        //! Returns the delay in microseconds.
        [[nodiscard]] int64_t getDelay() const
        {
            return m_delay.count();
        }

      private:
        std::chrono::microseconds m_delay;   //!< delay in microseconds
    };
}   // namespace vslib
