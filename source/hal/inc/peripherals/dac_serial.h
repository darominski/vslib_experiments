#pragma once

#include <stdint.h>

#include "cheby_gen/pb_top.h"

class DacSerial
{
  public:
    constexpr DacSerial(dac_serial_regs volatile& base) noexcept
        : m_regs(base)
    {
    }

    void set_config(
        int cpol, int cpha, int msb_first, int pre_pad, int post_pad, int data_width, int dead_time, int clk_pres,
        int fix_add_clk_cyc = 0
    )
    {
        int val = 0;
        val     |= cpol ? DAC_SERIAL_REGS_CONFIG_CPOL : 0;
        val     |= cpha ? DAC_SERIAL_REGS_CONFIG_CPHA : 0;
        val     |= msb_first ? DAC_SERIAL_REGS_CONFIG_MSB_FIRST : 0;
        val     |= (pre_pad << DAC_SERIAL_REGS_CONFIG_PRE_PAD_SHIFT);
        val     |= (post_pad << DAC_SERIAL_REGS_CONFIG_POST_PAD_SHIFT);
        val     |= (data_width << DAC_SERIAL_REGS_CONFIG_DATA_WIDTH_SHIFT);
        val     |= (dead_time << DAC_SERIAL_REGS_CONFIG_DEAD_TIME_SHIFT);
        val     |= (clk_pres << DAC_SERIAL_REGS_CONFIG_CLK_PRES_SHIFT);
        val     |= (fix_add_clk_cyc << DAC_SERIAL_REGS_CONFIG_FIX_ADD_CLK_CYC_SHIFT);

        m_regs.config = val;
    }

    void reset()
    {
        m_regs.ctrl = DAC_SERIAL_REGS_CTRL_RESET;
    }

    void busy_wait_while_busy()
    {
        for (;;)
        {
            if ((m_regs.status & DAC_SERIAL_REGS_STATUS_BUSY) == 0)
            {
                break;
            }
        }
    }

    void write_data(uint32_t data)
    {
        m_regs.data[0].value = data;
    }

    void start()
    {
        m_regs.ctrl = DAC_SERIAL_REGS_CTRL_START;
    }

  private:
    struct dac_serial_regs volatile& m_regs;
};
