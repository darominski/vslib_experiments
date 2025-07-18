#pragma once

#include "hal/inc/peripherals/uart.hpp"

class FiringUart
{
    enum class Thyristor
    {
        ABP = 0,   // CP AB+
        ABM = 1,   // CP AB-
        BCP = 2,   // CP BC+
        BCM = 3,   // CP BC-
        CAP = 4,   // CP CA+
        CAM = 5    // CP CA-
    };

  public:
    FiringUart(const uint32_t send_time_ns, const uint32_t send_time_bit_sc)
        : m_syncUart(send_time_ns, send_time_bit_sc, true)
    {
    }

    void sendData(const uint32_t time_s, const uint32_t time_ns, Thyristor thyristor)
    {
        m_syncUart.setFiringTime(time_s, time_ns);

        uint32_t thyristor_select = 0;
        thyristor_select          |= (1 << static_cast<uint32_t>(thyristor));
        m_syncUart.setData(thyristor_select);
    }

    void triggerDataTransfer()
    {
        m_syncUart.triggerSend();
    }

  private:
    hal::SyncUart m_syncUart;
};