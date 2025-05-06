//! Class for direct memory read/write access from bare-metal

#pragma once

#include <cstdint>

namespace hal
{

    class Bus
    {
      public:
        // Constructor takes a known memory address (e.g., FPGA base address)
        constexpr Bus(uintptr_t base_addr, size_t len)
            : base(reinterpret_cast<volatile uint8_t*>(base_addr)),
              len(len)
        {
        }

        // Direct access to the memory-mapped data
        volatile void* data() const
        {
            return base;
        }

        size_t size() const
        {
            return len;
        }

        uint32_t read(size_t offset) const
        {
            return *reinterpret_cast<volatile uint32_t*>(base + offset);
        }

        void write(size_t offset, uint32_t value)
        {
            *reinterpret_cast<volatile uint32_t*>(base + offset) = value;
        }

        explicit operator bool() const
        {
            return base != nullptr;
        }

      private:
        volatile uint8_t* base;
        size_t            len;
    };

}   // namespace hal