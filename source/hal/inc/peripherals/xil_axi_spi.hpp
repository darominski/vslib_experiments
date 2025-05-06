// xil_axi_spi.hpp
#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "bus.hpp"

namespace hal
{

    class XilAxiSpi
    {
        static constexpr uint32_t SRR_ADDR         = 0x40;   // Software reset register
        static constexpr uint32_t SPICR_ADDR       = 0x60;   // SPI control register
        static constexpr uint32_t SPISR_ADDR       = 0x64;   // SPI status register
        static constexpr uint32_t SPIDTR_ADDR      = 0x68;   // SPI data transmit register
        static constexpr uint32_t SPIDRR_ADDR      = 0x6C;   // SPI data receive register
        static constexpr uint32_t SPISSR_ADDR      = 0x70;   // SPI slave select register
        static constexpr uint32_t TX_FIFO_OCR_ADDR = 0x74;   // Transmit FIFO occupancy register
        static constexpr uint32_t RX_FIFO_OCR_ADDR = 0x78;   // Receive FIFO occupancy register
        static constexpr uint32_t DGIER_ADDR       = 0x1C;   // Device global interrupt enable
        static constexpr uint32_t IPISR_ADDR       = 0x20;   // IP interrupt status register
        static constexpr uint32_t IPIER_ADDR       = 0x28;   // IP interrupt enable register

        static constexpr uint32_t LOOP_BIT          = 0;
        static constexpr uint32_t SPE_BIT           = 1;
        static constexpr uint32_t MASTER_BIT        = 2;
        static constexpr uint32_t CPOL_BIT          = 3;
        static constexpr uint32_t CPHA_BIT          = 4;
        static constexpr uint32_t TX_FIFO_RESET_BIT = 5;
        static constexpr uint32_t RX_FIFO_RESET_BIT = 6;
        static constexpr uint32_t MANUAL_SS_BIT     = 7;
        static constexpr uint32_t TRANS_INHIBIT_BIT = 8;
        static constexpr uint32_t LSB_FIRST_BIT     = 9;

      public:
        XilAxiSpi(Bus& bus, uint32_t base, int fifo_size = 16)
            : m_bus(bus),
              m_base(base),
              m_fifo_size(fifo_size)
        {
            if (fifo_size != 16 && fifo_size != 256)
            {
                throw std::invalid_argument("fifo_size must be 16 or 256");
            }
        }

        void reset()
        {
            write(SRR_ADDR, 0xA);
        }

        void configure_interrupts(bool enable_global = true, bool rx_full = false, bool tx_empty = false)
        {
            uint32_t ipier_val = 0;
            if (rx_full) ipier_val |= (1 << 4);
            if (tx_empty) ipier_val |= (1 << 2);
            write(IPIER_ADDR, ipier_val);
            if (enable_global) write(DGIER_ADDR, 0x1);
        }

        void configure_core(
            bool master_mode = true, int cpol = 0, int cpha = 0, bool lsb_first = false, bool manual_ss = true,
            bool enable = true
        )
        {
            uint32_t val = 0;
            if (master_mode)
            {
                val |= (1 << MASTER_BIT);
            }
            if (cpol)
            {
                val |= (1 << CPOL_BIT);
            }
            if (cpha)
            {
                val |= (1 << CPHA_BIT);
            }
            if (lsb_first)
            {
                val |= (1 << LSB_FIRST_BIT);
            }
            if (manual_ss)
            {
                val |= (1 << MANUAL_SS_BIT);
            }
            if (enable)
            {
                val |= (1 << SPE_BIT);
            }
            val |= (1 << TRANS_INHIBIT_BIT);

            m_spicr_val = val;
            std::cout << m_spicr_val << '\n';
            write(SPICR_ADDR, m_spicr_val);
        }

        void reset_fifos()
        {
            uint32_t val = m_spicr_val | (1 << TX_FIFO_RESET_BIT) | (1 << RX_FIFO_RESET_BIT);
            write(SPICR_ADDR, val);
        }

        void start_transfer()
        {
            m_spicr_val &= ~(1 << TRANS_INHIBIT_BIT);
            write(SPICR_ADDR, m_spicr_val);
        }

        void inhibit_transfer()
        {
            m_spicr_val |= (1 << TRANS_INHIBIT_BIT);
            write(SPICR_ADDR, m_spicr_val);
        }

        void set_slave_select(uint32_t mask)
        {
            write(SPISSR_ADDR, mask & 0xFFFFFFFF);
        }

        void write_data(const std::vector<uint32_t>& data)
        {
            int tx_space = m_fifo_size - read(TX_FIFO_OCR_ADDR);
            if (data.size() > static_cast<size_t>(tx_space))
            {
                throw std::runtime_error("TX FIFO overflow");
            }
            for (const auto& byte : data)
            {
                write(SPIDTR_ADDR, byte & 0xFF);
            }
        }

        std::vector<uint8_t> read_rx_data()
        {
            int                  rx_count = read(RX_FIFO_OCR_ADDR) + 1;
            std::vector<uint8_t> data;
            data.reserve(rx_count);
            for (int i = 0; i < rx_count; ++i)
            {
                data.push_back(read(SPIDRR_ADDR));
            }
            return data;
        }

        uint32_t get_status()
        {
            return read(SPISR_ADDR);
        }

        bool is_tx_empty()
        {
            return get_status() & (1 << 2);
        }

        bool is_rx_full()
        {
            return get_status() & (1 << 1);
        }

        uint32_t get_interrupt_status()
        {
            return read(IPISR_ADDR);
        }

        void clear_interrupt(uint32_t status)
        {
            write(IPISR_ADDR, status);
        }

        void wait_for_transfer_complete()
        {
            while (!is_tx_empty())
            {
            }
        }

      private:
        Bus&     m_bus;
        uint32_t m_base;
        uint32_t m_spicr_val{0};
        int      m_fifo_size;

        uint32_t read(uint32_t addr)
        {
            return m_bus.read(m_base + addr);
        }

        void write(uint32_t addr, uint32_t data)
        {
            m_bus.write(m_base + addr, data);
        }
    };
}   // namespace hal