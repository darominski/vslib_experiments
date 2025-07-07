//! @file
//! @brief Defines the hardware abstraction layer for a ADC IP core.
//! @author Dominik Arominski

#pragma once

#include "cheby_gen/mb_top_singleton.hpp"

namespace hal
{
    class UncalibratedADC
    {
        // constants reimplemented from the Python cheby exports used by CCE:
        constexpr static uint32_t max_din_ports = 16;
        constexpr static uint32_t busy_src_ext  = 0;
        constexpr static uint32_t busy_src_sdo  = 1;
        constexpr static uint32_t busy_src_time = 2;

        // Constant holding the number of ADC ports
        constexpr static uint32_t adc_number_ports = ipCores::Top::AdcUncalintArrayItem::Adc::DataArray::size;

      public:
        //! Constructor for an uncalibrated ADC controller.
        UncalibratedADC(const uint32_t adc_id) noexcept
        {
            m_regs = hal::Top::instance().adcUncalint[adc_id].adc;
            // TMP: IP core needs to be configured before use, eventually this will be handled by FGC4 configurator
            setConfig(true, true, true, false, true, busy_src_ext, 0, false, 16, false);
            // END OF TMP
        }

        //! Sets the desired configuration of the serial (uncalibrated) ADC.
        //!
        //! @param clk_pol Clock polarity (0 - clock idle low, 1 - clock idle high)
        //! @param clk_ph Clock phase (0 - latch on leading edge, 1 - trailing edge)
        //! @param cnv_pol Polarity of ADC CNV input
        //! @param cnv_with_cs CNV works also as CS signal
        //! @param busy_pol Polarity of ADC BUSY output
        //! @param busy_srcSource of BUSY signal. 0 - dedicated BUSY port, 1 - SDO data line, 2 - internal timer
        //! @param busy_time When busy_src is set to internal timer, number of SPI clock cycles the controller will
        //! spend in emulated BUSY state. Ignored otherwise.
        //! @param clk_act_in_cnv SPI clock active (or not) during conversion/BUSY state
        //! @param data_width Width (in bits) of data received from ADC. Depends on target ADC chip
        //! @param gw_ctrl The ADC can be started by software (by calling the 'start' function) or through a gateware
        //! port. 0 = SW, 1 = GW
        void setConfig(
            const bool clk_pol, const bool clk_ph, const bool cnv_pol, const bool cnv_with_cs, const bool busy_pol,
            const uint8_t busy_src, const uint8_t busy_time, const bool clk_act_in_cnv, const uint8_t data_width,
            const bool gw_ctrl
        )
        {
            m_regs.config.cpol.set(clk_pol);
            m_regs.config.cpha.set(clk_ph);
            m_regs.config.cnvPol.set(cnv_pol);
            m_regs.config.cnvWithCs.set(cnv_with_cs);
            m_regs.config.busyPol.set(busy_pol);

            m_regs.config.busySrc.set(busy_src);
            m_regs.config.busyTime.set(busy_time);
            m_regs.config.clkActInConv.set(clk_act_in_cnv);
            m_regs.config.dataWidth.set(data_width);
            m_regs.config.gwCtrl.set(gw_ctrl);
        }

        //! Reset the ADC controller.
        void reset() noexcept
        {
            m_regs.ctrl.reset.set(true);
        }

        //! Reset the external ADC (hardware-dependent feature).
        void resetHardware() noexcept
        {
            m_regs.ctrl.hwReset.set(true);
            sleep(0.001);
            m_regs.ctrl.write(0x0);
        }

        //! Start conversion and transmission from ADC, block until done.
        void start()
        {
            m_regs.ctrl.start.set(true);
            uint32_t counter = 0;
            while (!m_regs.status.busy.get())
            {
                sleep(0.001);
                // infinite loop possible while waiting? limit this to a couple of seconds wait?
                if (counter++ > 1'000)   // 5 s wait
                {
                    throw std::runtime_error("ADC start timeout after 1 s.\n");
                }
            }
        }

        //! Read data received from an ADC port.
        //!
        //! @param port_index Port index
        //! @return Raw data received from an ADC port
        uint32_t read(const uint32_t port_index) noexcept
        {
            return m_regs.data[port_index].value.read();
        }

        //! Reads data from all ports sequentially and returns the read values.
        //!
        //! @return Array with the read raw ADC values
        const std::array<uint32_t, adc_number_ports>& readAllPorts() noexcept
        {
            for (uint32_t index = 0; index < adc_number_ports; index++)
            {
                m_values[index] = read(index);
            }
            return m_values;
        }

        //! Reads a value from the ADC and converts it to a user-understandable scale
        //!
        //! @param port_index Port index
        //! @return Converted data received from an ADC port
        float readConverted(const uint32_t port_index) noexcept
        {
            const uint32_t raw             = read(port_index);
            // check if the measured value is positive or negative, and shift accordingly:
            const int16_t   signed_sample  = static_cast<int16_t>(raw & 0xFFFF);
            constexpr float scaling_factor = 381.44e-6;
            return static_cast<int32_t>(signed_sample) * scaling_factor;
        }

      private:
        ipCores::Top::AdcUncalintArrayItem::Adc m_regs;   //!< IP core with register definitions

        std::array<uint32_t, adc_number_ports> m_values;        //!< Array holding all port values from the ADC
        std::array<float, adc_number_ports>    m_values_conv;   //!< Array holding all port values from the ADC
    };

}   // namespace hal