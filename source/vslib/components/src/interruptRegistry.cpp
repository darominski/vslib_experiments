//! @file
//! @brief Source file with interrupt registry used to register interrupts to the correct physical interrupt line
//! @author Dominik Arominski

#include <fmt/format.h>
#include <stdexcept>

#include "errorCodes.h"
#include "errorMessage.h"
#include "interruptRegistry.h"

namespace vslib
{

    void InterruptRegistry::registerInterrupt(
        std::string_view interrupt_name, std::function<void(void)> handler_function, int interrupt_id,
        InterruptPriority priority
    ) noexcept
    {
        if (m_interrupts.find(interrupt_name) != m_interrupts.end())
        {
            fgc4::utils::Error error_message(
                fmt::format("Interrupt with name: {} already registered!", interrupt_name),
                fgc4::utils::errorCodes::name_already_used
            );
            throw std::runtime_error("Interrupt name already registered!");
        }
        PeripheralInterrupt interrupt(interrupt_name, this, handler_function, interrupt_id, priority);
        m_interrupts.emplace(interrupt_name, interrupt);
    }

    void InterruptRegistry::startInterrupt(std::string_view interrupt_name) noexcept
    {
        getInterrupt(interrupt_name).start();
    }

    void InterruptRegistry::stopInterrupt(std::string_view interrupt_name) noexcept
    {
        getInterrupt(interrupt_name).stop();
    }

    PeripheralInterrupt& InterruptRegistry::getInterrupt(std::string_view interrupt_name) noexcept
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

}   // namespace vslib