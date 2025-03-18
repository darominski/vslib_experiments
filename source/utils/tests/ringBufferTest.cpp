//! @file
//! @brief File with RingBuffer unit tests.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "ringBufferAllocator.hpp"

using namespace fgc4::utils;

class RingBufferTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST(RingBufferTest, BasicIntsAllocation)
{
    // Allocate memory for some of the supported integer types
    const size_t n_objects_to_allocate = 2;

    // uint_8
    const size_t uint8_buffer_size = sizeof(uint8_t) * n_objects_to_allocate * 2;
    auto* uint8_ptr = RingBuffer<uint8_t, uint8_buffer_size>::template do_allocate<uint8_t>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(uint8_ptr) % alignof(uint8_t) == 0);   // Verifies alignment

    // int_8
    const size_t int8_buffer_size = sizeof(int8_t) * n_objects_to_allocate * 2;
    auto*        int8_ptr = RingBuffer<int8_t, int8_buffer_size>::template do_allocate<int8_t>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(int8_ptr) % alignof(int8_t) == 0);   // Verifies alignment

    // uint_32
    const size_t uint32_buffer_size = sizeof(uint32_t) * n_objects_to_allocate * 2;
    auto* uint32_ptr = RingBuffer<uint32_t, uint32_buffer_size>::template do_allocate<uint32_t>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(uint32_ptr) % alignof(uint32_t) == 0);   // Verifies alignment

    // int_32
    const size_t int32_buffer_size = sizeof(int32_t) * n_objects_to_allocate * 2;
    auto* int32_ptr = RingBuffer<int32_t, int32_buffer_size>::template do_allocate<int32_t>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(int32_ptr) % alignof(int32_t) == 0);   // Verifies alignment

    // uint_64
    const size_t uint64_buffer_size = sizeof(uint64_t) * n_objects_to_allocate * 2;
    auto* uint64_ptr = RingBuffer<uint64_t, uint64_buffer_size>::template do_allocate<uint64_t>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(uint64_ptr) % alignof(uint64_t) == 0);   // Verifies alignment

    // int_64
    const size_t int64_buffer_size = sizeof(int64_t) * n_objects_to_allocate * 2;
    auto* int64_ptr = RingBuffer<int64_t, int64_buffer_size>::template do_allocate<int64_t>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(int64_ptr) % alignof(int64_t) == 0);   // Verifies alignment
}

TEST(RingBufferTest, BasicCharAllocation)
{
    const size_t n_objects_to_allocate = 2;
    const size_t buffer_size           = sizeof(char) * (n_objects_to_allocate + 1);
    auto*        ptr = RingBuffer<char, buffer_size>::template do_allocate<char>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(ptr) % alignof(char) == 0);   // Verifies alignment
}

TEST(RingBufferTest, BasicStringAllocation)
{
    const size_t n_objects_to_allocate = 1;
    const size_t buffer_size           = sizeof(std::string) * n_objects_to_allocate * 2;
    auto*        ptr = RingBuffer<std::string, buffer_size>::template do_allocate<std::string>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(ptr) % alignof(char) == 0);   // Verifies alignment
}

TEST(RingBufferTest, BasicArrayAllocation)
{
    const size_t n_objects_to_allocate = 2;
    using array                        = std::array<double, 5>;
    const size_t buffer_size           = sizeof(array) * (n_objects_to_allocate + 1);
    auto*        ptr = RingBuffer<array, buffer_size>::template do_allocate<array>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(ptr) % alignof(double) == 0);   // Verifies alignment
}

TEST(RingBufferTest, BasicVectorAllocation)
{
    const size_t n_objects_to_allocate = 2;
    using vector                       = std::vector<double>;
    const size_t buffer_size           = sizeof(vector) * (n_objects_to_allocate + 1);
    auto*        ptr = RingBuffer<vector, buffer_size>::template do_allocate<vector>(n_objects_to_allocate);
    ASSERT_TRUE(reinterpret_cast<std::uintptr_t>(ptr) % alignof(double) == 0);   // Verifies alignment
}

TEST(RingBufferTest, RingBufferWrapAround)
{
    // Allocate memory until the ring buffer wraps around
    const size_t max_number_objects = 100;
    const size_t buffer_size        = sizeof(int) * max_number_objects;
    for (size_t i = 0; i < max_number_objects; ++i)
    {
        auto* ptr = RingBuffer<int, buffer_size>::template do_allocate<int>(1);
        EXPECT_TRUE(reinterpret_cast<std::uintptr_t>(ptr) % alignof(int) == 0);
    }

    // Verify that allocation continues correctly
    int* ptr = RingBuffer<int, buffer_size>::template do_allocate<int>(1);
    EXPECT_TRUE(reinterpret_cast<std::uintptr_t>(ptr) % alignof(int) == 0);
}

TEST(RingBufferTest, LargeMemoryRequest)
{
    // Test allocating memory chunks larger than BufferSize
    const size_t  max_number_objects = 5;
    const size_t  buffer_size        = sizeof(int) * max_number_objects;
    constexpr int large_size         = max_number_objects + 1;
    EXPECT_THROW((RingBuffer<int, buffer_size>::template do_allocate<int>(large_size)), std::bad_alloc);
}

TEST(RingBufferTest, MemoryClear)
{
    // Allocate some memory and then clear the ring buffer
    const size_t max_number_objects = 6;
    const size_t buffer_size        = sizeof(int) * max_number_objects;
    int*         ptr                = RingBuffer<int, buffer_size>::template do_allocate<int>(max_number_objects - 1);
    RingBuffer<int, buffer_size>::clear();

    // Verify that memory position resets to the beginning
    int* new_ptr = RingBuffer<int, buffer_size>::template do_allocate<int>(max_number_objects - 1);
    EXPECT_TRUE(ptr != nullptr);
    EXPECT_TRUE(new_ptr != nullptr);
    EXPECT_EQ(ptr, new_ptr);
}
