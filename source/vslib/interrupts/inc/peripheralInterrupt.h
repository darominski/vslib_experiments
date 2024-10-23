//! @file
//! @brief File containing a thin-layer interface to configure a peripheral interrupt.
//! @author Dominik Arominski

#pragma once

#include "bmboot/payload_runtime.hpp"
#include "interrupt.h"

namespace vslib
{
    //! This enumeration controls the interrupt priority by connecting to the appropriate interrupt line
    enum class InterruptPriority
    {
        high,
        medium,
        low,
    };

    // ************************************************************

    template<class Converter>
    class PeripheralInterrupt : public Interrupt<Converter>
    {
      public:
        //! Constructor for PeripheralInterrupt.
        //!
        //! @param handler_function Function to be called when the interrupt triggers
        //! @param interrupt_id Platform-dependent interrupt ID
        //! @param priority Priority level of the interrupt
        PeripheralInterrupt(
            std::string_view name, Converter& converter, const int interrupt_id, const InterruptPriority priority,
            std::function<void(Converter&)> handler_function
        )
            : Interrupt<Converter>(name, converter, std::move(handler_function)),
              m_interrupt_id{interrupt_id},
              m_priority{priority}
        {
            static_assert(
                std::derived_from<Converter, IConverter>,
                "The interrupt's template class must be derived from IConverter."
            );
            translatePriority();
            bmboot::setupInterruptHandling(m_interrupt_id, m_priority_bmboot, this->m_interrupt_handler);
        }

        //! Starts peripheral interrupt.
        void start() override
        {
            bmboot::enableInterruptHandling(m_interrupt_id);
        }

        //! Stops the peripheral interrupt from triggering.
        void stop() override
        {
            bmboot::disableInterruptHandling(m_interrupt_id);
        }

      private:
        int                              m_interrupt_id;      //!< Interrupt ID
        InterruptPriority                m_priority;          //!< Interrupt priority level
        bmboot::PayloadInterruptPriority m_priority_bmboot;   //!< Interrupt priority on the BMboot side

        //! Translates local enumeration of priority lines to the bmboot-internal enumeration of priority values.
        void translatePriority() noexcept
        {
            switch (m_priority)
            {
                case InterruptPriority::high:
                    m_priority_bmboot = bmboot::PayloadInterruptPriority::p7_max;
                    break;
                case InterruptPriority::medium:
                    m_priority_bmboot
                        = bmboot::PayloadInterruptPriority::p3;   // example of mapping medium to an intermediate value
                    break;
                case InterruptPriority::low:
                    m_priority_bmboot = bmboot::PayloadInterruptPriority::p0_min;
                    break;
            }
        }
    };
}   // namespace vslib
