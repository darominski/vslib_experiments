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
        //! Constructor for TimerInterrupt Component.
        //!
        //! @param name Name of this Interrupt Component
        //! @param parent Parent of this Interrupt Component
        //! @param handler_function Function to be called when an interrupt triggers
        TimerInterrupt(
            std::string_view name, Component* parent, Converter& converter,
            std::function<void(Converter&)> handler_function =
                [](Converter&)
            {
                ;
            }
        )
            : Interrupt<Converter>("TimerInterrupt", name, parent, converter, handler_function),
              delay(*this, "delay", 0.0)
        {
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

        //! Method called whenever any Parameter of this Component is modified.
        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            bmboot::setupPeriodicInterrupt(std::chrono::microseconds(delay.value()), this->m_interrupt_handler);

            return {};
        }

        Parameter<int64_t> delay;   //!< delay in microseconds
    };
}   // namespace vslib
