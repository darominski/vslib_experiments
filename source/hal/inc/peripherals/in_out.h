#pragma once

#include <array>
#include <stdint.h>
#include <stdio.h>

#include "cheby_gen/pb_top.h"

class InOut
{
  public:
    constexpr InOut(in_out_regs volatile& base) noexcept
        : m_regs(base)
    {
    }

    void set_dir(int dir)
    {
        m_regs.DDR = dir;
    }

    void set_output(uint32_t word)
    {
        m_regs.POR = word;
    }

    uint32_t get_input()
    {
        return m_regs.PIR;
    }

    uint32_t get_output()
    {
        return m_regs.POR;
    }

  private:
    struct in_out_regs volatile& m_regs;
};
