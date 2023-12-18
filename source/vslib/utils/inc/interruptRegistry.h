//! @file
//! @brief Header file with interrupt registry used to register interrupts to the physical interrupt line
//! @author Dominik Arominski

#pragma once

#include <map>

#include "peripheralInterrupt.h"

namespace vslib
{
    class InterruptRegistry
    {
      public:
        InterruptRegistry() noexcept = default;

        void registerInterrupt(std::string_view, std::function<void(void)>, int, InterruptPriority) noexcept;

        void startInterrupt(std::string_view) noexcept;
        void stopInterrupt(std::string_view) noexcept;

      private:
        // holds all Peripheral interrupts
        std::map<std::string_view, std::reference_wrapper<PeripheralInterrupt>> m_interrupts;

        PeripheralInterrupt& getInterrupt(std::string_view) noexcept;
    };

}   // namespace vslib