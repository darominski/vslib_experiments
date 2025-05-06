//! @file
//! @brief Defines the hardware abstraction layer for a ADC IP core.
//! @author Dominik Arominski

#pragma once

#include "cheby_gen/mb_top.hpp"

namespace hal
{
    // constants reimplemented from the Python cheby exports used by CCE:
    constexpr uint32_t max_din_ports = 16;
    constexpr uint32_t busy_src_ext  = 0;
    constexpr uint32_t busy_src_sdo  = 1;
    constexpr uint32_t busy_src_time = 2;

    template<uint32_t adc_id>
    class UncalibratedADC
    {
      public:
        UncalibratedADC(uint8_t data_width = 16) noexcept
        {
            ipCores::Top top(reinterpret_cast<uint8_t*>(0xa0000000));
            m_regs = top.adcUncalint[adc_id].adc;

            // TMP: Configure an uncalibrated ADC manually, until the Configurator is developed
            setConfig(true, true, true, false, false, busy_src_ext, 0, false, data_width, false);
            // END OF TMP
        }

        void setConfig(
            const bool cpol, const bool cpha, const bool cnv_pol, const bool cnv_with_cs, const bool busy_pol,
            const uint8_t busy_src, const uint8_t busy_time, const bool clk_act_in_cnv, const uint8_t data_width,
            const bool gw_ctrl
        )
        {
            m_regs.config.cpol.set(cpol);
            m_regs.config.cpha.set(cpha);
            m_regs.config.cnvPol.set(cnv_pol);
            m_regs.config.cnvWithCs.set(cnv_with_cs);
            m_regs.config.busyPol.set(busy_pol);

            m_regs.config.busySrc.set(busy_src);
            m_regs.config.busyTime.set(busy_time);
            m_regs.config.clkActInConv.set(clk_act_in_cnv);
            m_regs.config.dataWidth.set(data_width);
            m_regs.config.gwCtrl.set(gw_ctrl);
        }

        void reset() noexcept
        {
            m_regs.ctrl.reset.set(true);
        }

        void resetHardware() noexcept
        {
            m_regs.ctrl.hwReset.set(true);
            sleep(0.001);
            m_regs.ctrl.write(0x0);
        }

        void start()
        {
            m_regs.ctrl.start.set(true);
            uint32_t counter = 0;
            while (!m_regs.status.busy.get())
            {
                sleep(0.001);
                // infinite loop possible while waiting? limit this to a couple of seconds wait?
                if (counter++ > 5'000)   // 5 s wait
                {
                    throw std::runtime_error("ADC start timeout after 5 s.\n");
                }
            }
        }

        uint32_t read(const uint32_t channel_index) noexcept
        {
            return m_regs.data[channel_index].value.read();
        }

        const std::array<uint32_t, 16>& readAllChannels() noexcept
        {
            for (int index = 0; index < 16; index++)
            {
                m_values[index] = read(index);
            }
            return m_values;
        }

        float readConverted(const uint32_t channel_index) noexcept
        {
            const uint32_t raw          = read(channel_index);
            // check if the measured value is positive or negative, and shift accordingly:
            const int16_t signed_sample = static_cast<int16_t>(raw & 0xFFFF);
            return static_cast<int32_t>(signed_sample) * 381.44e-6;
        }

      private:
        ipCores::Top::AdcUncalintArrayItem::Adc m_regs;

        std::array<uint32_t, 16> m_values;
    };

}   // namespace hal