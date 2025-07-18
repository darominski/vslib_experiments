//! @file
//! @brief Defines the hardware abstraction layer for a Synchronised UART IP core.
//! @author Dominik Arominski

#pragma once

#include "cheby_gen/mb_top_singleton.hpp"

namespace hal
{
    class SyncUart
    {
      public:
        //! Constructor for PWM HAL object.
        SyncUart(const uint32_t send_time_ns, const uint32_t send_time_bit_sc, const bool enable_parity) noexcept
        {
            m_regs = hal::Top::instance().syncUart;
            //! TMP code to configure the HAL in absence of the Configurator
            setConfiguration(send_time_ns, send_time_bit_sc, enable_parity);
            //! end TMP code
        }

        //! Configures the IP core in absence of the Configurator.
        //!
        //! @param send_time_ns Time required to send a UART frame in ns
        //! @param send_time_bit_sc Time required to send 1 bit in sc
        //! @param enable_parity Flag to enable parity bit (XOR)
        //! @param inter_frame_gap Number of idle symbols between two UART frames, max 127
        void setConfiguration(
            const uint32_t send_time_ns, const uint32_t send_time_bit_sc, const bool enable_parity,
            const uint32_t inter_frame_gap = 2
        )
        {
            m_regs.uartSendTimeNs.write(send_time_ns);
            m_regs.scPerUartBit.write(send_time_bit_sc);
            m_regs.config.write(enable_parity);
            m_regs.config.write(inter_frame_gap);
        }

        //! Triggers sending a UART message frame now instead of waiting for a firing time.
        void triggerSend()
        {
            m_regs.setTxData.write(true);
        }

        //! Sets the firing time when the UART message frame will be sent.
        //!
        //! @param time_s Firing time, seconds part
        //! @param time_ns Firing time, nanoseconds part
        void setFiringTime(const uint32_t time_s, const uint32_t time_ns)
        {
            m_regs.firingTimeS.write(time_s);
            m_regs.firingTimeNs.write(time_ns);
        }

        //! Sets the data to be sent over UART.
        //!
        //! @param data Data to be sent, only the lower 8 bits are used
        void setData(const uint32_t data)
        {
            m_regs.uartData.write(data);
        }

      private:
        ipCores::Top::SyncUart m_regs;
    };
}