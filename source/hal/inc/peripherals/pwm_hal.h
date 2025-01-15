#pragma once

#include "cheby_gen/pwm_regs.h"

namespace hal
{

    class PWM
    {
      public:
        constexpr PWM(uint8_t* base_address) noexcept
            : m_regs(base_address)
        {
        }

      private:
        myModule::PwmRegs volatile& m_regs;
    };
}