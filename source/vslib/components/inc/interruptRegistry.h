//! @file
//! @brief Header file with interrupt registry used to register interrupts to the physical interrupt line
//! @author Dominik Arominski

#pragma once

#include <map>
#include <string>

#include "component.h"
#include "peripheralInterrupt.h"

namespace vslib
{
    class InterruptRegistry : public Component
    {
      public:
        //! Constructor for the InterruptRegistry Component.
        //!
        //! @param name Name of this Component
        //! @param parent Parent of this Component
        InterruptRegistry(std::string_view name, Component* parent) noexcept
            : Component("InterruptRegistry", name, parent)
        {
        }

        //! Creates an interrupt with provided parameters and registers it in the internal container under the given
        //! name id.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        //! @param handler_function Function to be called when this interrupt triggers
        //! @param interrupt_id Interrupt id to be given
        //! @param priority The physical priority line of the interrupt
        void registerInterrupt(
            std::string_view interrupt_name, std::function<void(void)> handler_function, int interrupt_id,
            InterruptPriority priority
        ) noexcept;

        //! Starts the chosen interrupt.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        void startInterrupt(std::string_view interrupt_name) noexcept;

        //! Stops the chosen interrupt.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        void stopInterrupt(std::string_view interrupt_name) noexcept;

      private:
        //!< Container with all registered Peripheral interrupts
        std::map<std::string_view, std::reference_wrapper<PeripheralInterrupt>> m_interrupts;

        //! Returns a reference to the chosen interrupt.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        PeripheralInterrupt& getInterrupt(std::string_view interrupt_name) noexcept;
    };

}   // namespace vslib