//! @file
//! @brief Defines the hardware abstraction layer and convenience methods to control AD7606C features.
//! @author Dominik Arominski

#pragma once

#include "uncalibrated_adc.hpp"
#include "xil_axi_spi.hpp"

namespace hal
{
    class AD7606C
    {
      public:
        //! Constructor for a AD7606c features.
        //!
        //! @param spi SPI controller (tested with Xilinx SPI)
        //! @param pin_index Slave select pin index
        //! @param adc Associated ADC controller
        AD7606C(XilAxiSpi& spi, uint32_t pin_index, UncalibratedADC& adc)
        noexcept
            : m_spi(spi),
              m_adc(adc),
              m_pin_index(pin_index)
        {
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
        XilAxiSpi       m_spi;         //!< HAL handle for SPI
        UncalibratedADC m_adc;         //!< HAL handle for an ADC to be configured
        uint32_t        m_pin_index;   //!< Index of the ADC pin

        //! Write data to a register
        //!
        //! @param address Register address
        //! @param data Data to be written
        void write_register(uint32_t address, uint32_t data)
        {
            if (address >= 0x2F)
            {
                throw std::out_of_range("Address out of valid register range (0x00 to 0x2F)");
            }
            // first two bits are /WEN and /WRRD
            m_spi.write_data({0x3F & address, data & 0xFF});
            usleep(1);
            m_spi.set_slave_select(~(0x1 << m_pin_index));
            usleep(1);
            m_spi.start_transfer();
            usleep(1);
            m_spi.wait_for_transfer_complete();
            usleep(1);
            m_spi.inhibit_transfer();
            usleep(1);
            m_spi.set_slave_select(~(0x0));
            usleep(1);
        }
    };
}   // namespace hal