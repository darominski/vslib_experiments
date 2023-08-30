//! @file
//! @brief Class defining fixed-size memory pool for allocating objects within the ring-buffer of the pool to avoid
//! memory fragmentation.
//! @author Dominik Arominski

#pragma once

#include <cstring>
#include <iostream>
#include <memory_resource>

namespace vslib::utils
{
    template<typename BufferType, uint64_t BufferSize>
    class RingBuffer
    {
      public:
        template<typename T>
        static T* do_allocate(const uint64_t bytes)
        {
            auto size       = sizeof(T) * bytes;
            auto adjustment = calculate_adjustment<T>();

            // Update the pointer to the next available memory location that is adjusted
            m_current_position += adjustment;

            // Check if there is enough memory available
            if (size + adjustment >= BufferSize)   // too big memory chunk required
            {
                throw std::bad_alloc();   // Which should trip the converter
            }
            else
            {
                if (m_current_position + size >= BufferSize)
                {
                    m_current_position = 0;   // Overflow: wraps around to the beginning of the buffer
                    m_current_position += calculate_adjustment<T>();   // needs to recalculate adjustment
                }
                // Allocate memory by returning a pointer to the adjusted memory location
                void* ptr          = m_buffer + m_current_position;
                // Update the position inside memory
                m_current_position += size;
                return reinterpret_cast<T*>(ptr);
            }
        }

        //! "Clears" the ring buffer by resetting the position to the beggining of the memory pool
        static void clear() noexcept
        {
            m_current_position = 0;
        }

        //! Calculates the adjustment needed to align the next memory allocation
        //! based on the current buffer position and the alignment requirement of
        //! the allocated type.
        //!
        //! @return The adjustment value for proper alignment.
        template<typename T>
        static auto calculate_adjustment() noexcept
        {
            constexpr auto alignment = alignof(T);
            return (alignment - (m_current_position % alignment)) % alignment;
        }

      private:
        static std::byte m_buffer[BufferSize];
        static uint64_t  m_current_position;
    };

    // ************************************************************

    template<typename T, typename BufferType, uint64_t BufferSize>
    class StaticRingBufferAllocator
    {
      public:
        using value_type = T;

        StaticRingBufferAllocator() noexcept = default;

        template<typename U>
        explicit StaticRingBufferAllocator(const StaticRingBufferAllocator<U, BufferType, BufferSize>&) noexcept
        {
        }

        //! Allows the allocator to be rebound for different types. This is essential for compatibility
        //! when the allocator is used with containers storing elements of varying types.
        //! @tparam U The new type to rebind the allocator to.
        template<typename U>
        struct rebind
        {
            using other = StaticRingBufferAllocator<U, BufferType, BufferSize>;
        };

        T* allocate(uint64_t count)
        {
            return RingBuffer<BufferType, BufferSize>::template do_allocate<T>(count);
        }

        void deallocate(T*, uint64_t) noexcept
        {
            // Deallocation is not needed, the pool is managed internally.
        }

        [[nodiscard]] uint64_t max_size() const noexcept
        {
            return BufferSize / sizeof(T);
        }
    };

}   // namespace utils