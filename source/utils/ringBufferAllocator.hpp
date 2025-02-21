//! @file
//! @brief Class defining fixed-size memory pool for allocating objects within the ring-buffer of the pool to avoid
//! memory fragmentation.
//! @author Dominik Arominski

#pragma once

#include <cstring>
#include <iostream>
#include <memory_resource>
#include <type_traits>

#include "fmt/format.h"

namespace fgc4::utils
{
    template<typename BufferType, uint64_t BufferSize>
    class RingBuffer
    {
      public:
        //! Allocates a requested number of objects of the templated type in the ring buffer
        //!
        //! @tparam T type of the object to be allocated
        //! @param count Number of objects to be allocated
        //! @return Pointer to the beggining of the memory region where the objects are allocated
        template<typename T>
        static T* do_allocate(const uint64_t count)
        {
            auto size       = sizeof(T) * count;
            auto adjustment = calculate_adjustment<T>();

            // Update the pointer to the next available memory location that is adjusted
            m_current_position += adjustment;

            // Check if there is enough memory available
            if (size + adjustment < BufferSize)   // too big memory chunk required
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
            else
            {
                std::cerr << fmt::format("Buffer too small to allocate the requested object.\n");
                throw std::bad_alloc();   // Which should trip the converter
            }
        }

        //! "Clears" the ring buffer by resetting the position to the beggining of the memory pool
        static void clear() noexcept
        {
            m_current_position = 0;
        }

      private:
        static std::byte       m_buffer[BufferSize];
        inline static uint64_t m_current_position = 0;

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
    };

    // ************************************************************

    template<typename T, typename BufferType, uint64_t BufferSize>
    class StaticRingBufferAllocator
    {
      public:
        using value_type                     = T;
        StaticRingBufferAllocator() noexcept = default;

        template<typename U>
        StaticRingBufferAllocator(const StaticRingBufferAllocator<U, BufferType, BufferSize>&) noexcept
        {
        }

        template<typename U, typename OtherBufferType, uint64_t OtherBufferSize>
        bool operator==(const StaticRingBufferAllocator<U, OtherBufferType, OtherBufferSize>&) const noexcept
        {
            // The buffers are the same if the type and size agrees, regardless of the stored type
            return (std::is_same_v<BufferType, OtherBufferType> && BufferSize == OtherBufferSize) ? true : false;
        }

        // Comparison operator for inequality
        template<typename U, typename OtherBufferType, uint64_t OtherBufferSize>
        bool operator!=(const StaticRingBufferAllocator<U, OtherBufferType, OtherBufferSize>& other) const noexcept
        {
            // Implement the logic to check if allocators are not equal.
            return !(*this == other);
        }

        //! Allows the allocator to be rebound for different types. This is essential for compatibility
        //! when the allocator is used with containers storing elements of varying types.
        //! @tparam U The new type to rebind the allocator to.
        template<typename U>
        struct rebind
        {
            using other = StaticRingBufferAllocator<U, BufferType, BufferSize>;
        };

        //! Allocates memory in the RingBuffer for a provided numer of objects of the type T.
        //! Satisfies allocator_traits,
        //!
        //! @param count Number of objects intended to be allocated
        //! @return Pointer to the location of RingBuffer where objects can be allocated
        T* allocate(uint64_t count)
        {
            return RingBuffer<BufferType, BufferSize>::template do_allocate<T>(count);
        }

        //! Satisfies allocator_traits but memory deallocation is not foreseen while application runs
        void deallocate(T*, uint64_t) noexcept
        {
            // Deallocation is not needed, the pool is managed internally.
        }

        //! Provides maximal number of objects of type T that can be allocated given the predefined buffer size
        //!
        //! @return Number of objects that can be allocated in the buffer
        [[nodiscard]] uint64_t max_size() const noexcept
        {
            return BufferSize / sizeof(T);
        }
    };

    // ************************************************************
    // Initialization of static heaps for supported types

    // Let's align the buffers to the largest type we want to have in our JSON
    template<typename BufferType, size_t BufferSize>
    alignas(std::max_align_t) std::byte RingBuffer<BufferType, BufferSize>::m_buffer[];

}   // namespace fgc4::utils
