//! Class for direct memory read/write access from bare-metal

#pragma once

#include <cstdint>

namespace hal
{

    class Bus
    {
      public:
        // Constructor takes a known memory address (e.g., FPGA base address).
        //!
        //! @param base_addr Base address for memory access
        //! @param len Length of memory to be accessed
        constexpr Bus(uintptr_t base_addr, size_t len)
            : base(reinterpret_cast<volatile uint8_t*>(base_addr)),
              len(len)
        {
        }

        //! Returns a direct access to the memory-mapped data.
        //!
        //! @return Pointer to the base address of accessed memory
        volatile void* data() const
        {
            return base;
        }

        //! Returns size of the mapped memory.
        //!
        //! @return Size of the mapped memory
        size_t size() const
        {
            return len;
        }

        //! Read data a the chosen address.
        //!
        //! @param offset Offset from the base address to read from
        //! @return Value at the chosen address (as unsigned int)
        uint32_t read(size_t offset) const
        {
            return *reinterpret_cast<volatile uint32_t*>(base + offset);
        }

        //! Write a provided value to the chosen address.
        //!
        //! @param offset Offset from the base address to write to
        //! @param value Value to be written
        void write(size_t offset, uint32_t value)
        {
            *reinterpret_cast<volatile uint32_t*>(base + offset) = value;
        }

        //! Overrite allowing to check if the mapping has been successfull.
        explicit operator bool() const
        {
            return base != nullptr;
        }

      private:
        volatile uint8_t* base;
        size_t            len;
    };

}   // namespace hal