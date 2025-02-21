//! @file
//! @brief Header file with interrupt registry used to register interrupts to the physical interrupt line
//! @author Dominik Arominski

#pragma once

#include <map>
#include <string>

#include "peripheralInterrupt.hpp"

namespace vslib
{
    template<typename Converter>
    class InterruptRegistry
    {
      public:
        //! Constructor for the InterruptRegistry.
        InterruptRegistry() = default;

        //! Creates an interrupt with provided parameters and registers it in the internal container under the given
        //! name id.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        //! @param handler_function Function to be called when this interrupt triggers
        //! @param interrupt_id Interrupt id to be given
        //! @param priority The physical priority line of the interrupt
        void registerInterrupt(
            std::string_view interrupt_name, std::function<void(Converter&)> handler_function, const int interrupt_id,
            InterruptPriority priority
        )
        {
            static_assert(
                std::derived_from<Converter, IConverter>,
                "The interrupt's template class must be derived from IConverter."
            );
            if (m_interrupts.find(interrupt_name) != m_interrupts.end())
            {
                fgc4::utils::Error error_message(
                    fmt::format("Interrupt with name: {} already registered!", interrupt_name),
                    fgc4::utils::errorCodes::name_already_used
                );
                throw std::runtime_error("Interrupt name already registered!");
            }

            PeripheralInterrupt interrupt(interrupt_name, interrupt_id, priority, handler_function);
            m_interrupts.emplace(interrupt_name, interrupt);
        }

        //! Starts the chosen interrupt.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        void startInterrupt(std::string_view interrupt_name)
        {
            getInterrupt(interrupt_name).start();
        }

        //! Stops the chosen interrupt.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        void stopInterrupt(std::string_view interrupt_name)
        {
            getInterrupt(interrupt_name).stop();
        }

      private:
        //!< Container with all registered Peripheral interrupts
        std::map<std::string_view, std::reference_wrapper<PeripheralInterrupt>> m_interrupts;

        //! Returns a reference to the chosen interrupt.
        //!
        //! @param interrupt_name Name identifier for the peripheral interrupt
        PeripheralInterrupt& getInterrupt(std::string_view interrupt_name)
        {
            auto iterator = m_interrupts.find(interrupt_name);
            if (iterator == m_interrupts.end())
            {

                fgc4::utils::Error error_message(
                    fmt::format("Interrupt with name: {} has not been registered!", interrupt_name),
                    fgc4::utils::errorCodes::name_already_used
                );
                // possibly throw an exception to trip the converter since code is most likely severely malformed
                throw std::runtime_error("Interrupt has not been registered!");
            }
            return iterator->second.get();
        }
    };

}   // namespace vslib
