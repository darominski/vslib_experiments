#pragma once

#include <array>
#include <stdint.h>
#include <stdio.h>

#include "cheby_gen/pb_top.h"

constexpr int MAX_DIN_PORTS = 16;
using AdcReadout            = std::array<int32_t, MAX_DIN_PORTS>;

class AdcSerial
{
  public:
    constexpr AdcSerial(adc_serial_regs volatile& base) noexcept
        : m_regs(base),
          m_data_width(0)
    {
    }

    void set_config(
        int cpol, int cpha, int cnv_pol, int cnv_with_cs, int busy_pol, int busy_src, int busy_time, int clk_act_in_cnv,
        int data_width
    )
    {
        int val = 0;
        val     |= cpol ? ADC_SERIAL_REGS_CONFIG_CPOL : 0;
        val     |= cpha ? ADC_SERIAL_REGS_CONFIG_CPHA : 0;
        val     |= cnv_pol ? ADC_SERIAL_REGS_CONFIG_CNV_POL : 0;
        val     |= cnv_with_cs ? ADC_SERIAL_REGS_CONFIG_CNV_WITH_CS : 0;
        val     |= busy_pol ? ADC_SERIAL_REGS_CONFIG_BUSY_POL : 0;
        val     |= (busy_src << ADC_SERIAL_REGS_CONFIG_BUSY_SRC_SHIFT);
        val     |= (busy_time << ADC_SERIAL_REGS_CONFIG_BUSY_TIME_SHIFT);
        val     |= clk_act_in_cnv ? ADC_SERIAL_REGS_CONFIG_CLK_ACT_IN_CONV : 0;
        val     |= (data_width << ADC_SERIAL_REGS_CONFIG_DATA_WIDTH_SHIFT);
        printf("[%08lX] <= %08X\n", (uintptr_t)&m_regs.config, val);

        m_regs.config = val;
        m_data_width  = data_width;
    }

    void reset()
    {
        m_regs.ctrl = ADC_SERIAL_REGS_CTRL_RESET;
    }

    void start()
    {
        m_regs.ctrl = ADC_SERIAL_REGS_CTRL_START;

        //        printf("STATUS: %08X\n", m_regs.status);

        for (;;)
        {
            if ((m_regs.status & ADC_SERIAL_REGS_STATUS_BUSY) == 0)
            {
                break;
            }
        }
    }

    AdcReadout read_data()
    {
        AdcReadout data_out;
        for (int i = 0; i < MAX_DIN_PORTS; i++)
        {
            int ADC_WIDTH = 20;   // FIXME !

            auto raw_meas      = m_regs.data[i].value;
            auto sign_bit      = (1 << (ADC_WIDTH - 1));
            auto sign_ext_mask = ~((1 << ADC_WIDTH) - 1);
            auto sign_ext      = raw_meas | ((raw_meas & sign_bit) ? sign_ext_mask : 0);

            data_out[i] = sign_ext;
        }

        return data_out;
    }

  private:
    struct adc_serial_regs volatile& m_regs;
    int                              m_data_width;
};
