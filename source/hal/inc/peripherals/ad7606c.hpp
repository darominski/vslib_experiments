//! @file
//! @brief Defines the hardware abstraction layer and convenience methods to control AD7606C features.
//! @author Dominik Arominski

#pragma once

#include "uncalibrated_adc.hpp"
#include "xil_axi_spi.hpp"

namespace hal
{
    template<uint32_t adc_id>
    class AD7606C
    {
      public:
        AD7606C(XilAxiSpi& spi, uint32_t pin_index, UncalibratedADC<adc_id>& adc)
        noexcept
            : m_spi(spi),
              m_adc(adc),
              m_pin_index(pin_index)
        {
            // TMP: Configure an uncalibrated ADC manually, until the Configurator is developed
            // END OF TMP
            m_adc.reset();
            m_adc.resetHardware();
            lockSPIMode();
            configure();
        }

        //! Configure SPI master mode to work with this ADC.
        void lockSPIMode() noexcept
        {
            // master_mode = true, cpol = 1, cpha = 0, lsb_first = false,
            // manual_ss = true, enable = true
            m_spi.configure_core(true, 1, 0, false, true, true);
        }

        //! Configuree ADC register to match FGC4 mode.
        void configure() noexcept
        {
            write_register(0x2, 0x18);   // 8 DOUT output mode
            write_register(0x3, 0xAA);   // CH1,2 input 12.5V differential
            write_register(0x4, 0xAA);   // CH3,4 input 12.5V differential
            write_register(0x5, 0xAA);   // CH5,6 input 12.5V differential
            write_register(0x6, 0xAA);   // CH7,8 input 12.5V differential
            write_register(0x7, 0xFF);   // Enable high bandwidth on all channels
        }

      private:
        XilAxiSpi               m_spi;
        UncalibratedADC<adc_id> m_adc;
        uint32_t                m_pin_index;

        void write_register(uint32_t address, uint32_t data)
        {
            if (address >= 0x2F)
            {
                throw std::out_of_range("Address out of valid register range (0x00 to 0x2F)");
            }
            m_spi.write_data({0x3F & address, data & 0xFF});
            m_spi.set_slave_select(~(0x1 << m_pin_index));
            m_spi.start_transfer();
            m_spi.wait_for_transfer_complete();
            m_spi.inhibit_transfer();
            m_spi.set_slave_select(~(0x0));
        }
    };
}   // namespace hal