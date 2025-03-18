//! @file
//! @brief File with RingBufferAllocator unit tests.
//! @author Dominik Arominski

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "ringBufferAllocator.hpp"

using namespace fgc4::utils;

class RingBufferAllocatorTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST(StaticRingBufferAllocatorTest, BasicDoubleAllocation)
{
    constexpr size_t                                       buffer_size = sizeof(double) * 6;
    StaticRingBufferAllocator<double, double, buffer_size> allocator;
    double*                                                ptr = allocator.allocate(5);
    // Verify alignment and validity of the pointer
    EXPECT_TRUE(reinterpret_cast<std::uintptr_t>(ptr) % alignof(double) == 0);
}

TEST(StaticRingBufferAllocatorTest, AllocatorRebinding)
{
    constexpr size_t n_objects   = 6;
    constexpr size_t buffer_size = sizeof(int) * n_objects;

    StaticRingBufferAllocator<int, int, buffer_size> int_allocator;

    // Rebind the allocator for a different type
    using CharAllocator = StaticRingBufferAllocator<char, int, buffer_size>;
    CharAllocator char_allocator
        = std::allocator_traits<CharAllocator>::select_on_container_copy_construction(int_allocator);

    // Allocate memory for different types using the new allocator
    int*  int_ptr  = int_allocator.allocate(5);
    char* char_ptr = char_allocator.allocate(3);

    // Verify that the allocated memory is correctly aligned
    EXPECT_TRUE(reinterpret_cast<std::uintptr_t>(int_ptr) % alignof(int) == 0);
    EXPECT_TRUE(reinterpret_cast<std::uintptr_t>(char_ptr) % alignof(char) == 0);
}

TEST(StaticRingBufferAllocatorTest, MaxSize)
{
    constexpr size_t                                 n_objects   = 6;
    constexpr size_t                                 buffer_size = sizeof(int) * n_objects;
    StaticRingBufferAllocator<int, int, buffer_size> allocator;

    // Query max_size for different types
    EXPECT_EQ(allocator.max_size(), buffer_size / sizeof(int));
}

// Tests allocating a single basic_string
TEST(StaticRingBufferAllocatorTest, AllocateStrings)
{
    constexpr size_t n_characters = 100;
    constexpr size_t buffer_size  = sizeof(char) * n_characters;

    using StringAllocator = StaticRingBufferAllocator<char, std::string, buffer_size>;

    using StaticString = std::basic_string<char, std::char_traits<char>, StringAllocator>;

    StaticString test_string("test");
    EXPECT_EQ(test_string.size(), 4);
    EXPECT_EQ(test_string, "test");
}

// Tests allocating a large number of basic_strings
TEST(StaticRingBufferAllocatorTest, WrapAroundStringAllocation)
{
    constexpr size_t n_characters = 100;
    constexpr size_t buffer_size  = sizeof(char) * n_characters;

    using StringAllocator = StaticRingBufferAllocator<char, std::string, buffer_size>;

    using StaticString = std::basic_string<char, std::char_traits<char>, StringAllocator>;

    constexpr size_t test_characters = 4;   // length of string "test"
    for (size_t index = 0; index <= (n_characters / test_characters); index++)
    {
        StaticString test_string("test");
        EXPECT_EQ(test_string.size(), 4);
        EXPECT_EQ(test_string, "test");
    }
}

// Tests allocating too large basic_string
TEST(StaticRingBufferAllocatorTest, TooLargeStringAllocation)
{
    constexpr size_t n_characters = 110;
    constexpr size_t buffer_size  = sizeof(char) * n_characters;

    using StringAllocator = StaticRingBufferAllocator<char, char, buffer_size>;

    using StaticString = std::basic_string<char, std::char_traits<char>, StringAllocator>;
    // if buffer size is tied to the size of char, then the maximal number of characters is actually
    // n_characters / 2 - 1
    EXPECT_THROW(
        StaticString large_string(n_characters / 2, '*'), std::length_error
    );   // std::length_error from std::basic_string gets called before std::bad_alloc
}

TEST(StaticRingBufferAllocatorTest, AllocateVector)
{
    constexpr size_t max_n_elements = 10;
    constexpr size_t buffer_size    = sizeof(double) * max_n_elements;

    using VectorAllocator = StaticRingBufferAllocator<double, std::vector<double>, buffer_size>;

    std::vector<double, VectorAllocator> vec;
    vec.push_back(1.0);
    vec.push_back(2.0);
    vec.push_back(3.0);

    // Verify the vector's contents and size
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1.0);
    EXPECT_EQ(vec[1], 2.0);
    EXPECT_EQ(vec[2], 3.0);
}

TEST(StaticRingBufferAllocatorTest, ManyVectorsAllocation)
{
    constexpr size_t max_n_elements = 10;
    constexpr size_t buffer_size    = sizeof(double) * max_n_elements;

    using VectorAllocator = StaticRingBufferAllocator<double, std::vector<double>, buffer_size>;

    std::vector<double, VectorAllocator> vec1{1.0, 2.0, 3.0};
    std::vector<double, VectorAllocator> vec2{4.0, 5.0, 6.0};
    std::vector<double, VectorAllocator> vec3{7.0, 8.0, 9.0};

    // Verify the vector's contents and size
    EXPECT_EQ(vec1.size(), 3);
    EXPECT_EQ(vec2.size(), 3);
    EXPECT_EQ(vec3.size(), 3);

    EXPECT_EQ(vec1[0], 1.0);
    EXPECT_EQ(vec1[1], 2.0);
    EXPECT_EQ(vec1[2], 3.0);
    EXPECT_EQ(vec2[0], 4.0);
    EXPECT_EQ(vec2[1], 5.0);
    EXPECT_EQ(vec2[2], 6.0);
    EXPECT_EQ(vec3[0], 7.0);
    EXPECT_EQ(vec3[1], 8.0);
    EXPECT_EQ(vec3[2], 9.0);
}

TEST(StaticRingBufferAllocatorTest, TooManyVectorElementsAllocation)
{
    constexpr size_t max_n_elements = 10;
    constexpr size_t buffer_size    = sizeof(double) * max_n_elements;

    using VectorAllocator = StaticRingBufferAllocator<double, std::vector<double>, buffer_size>;

    std::vector<double, VectorAllocator> vec;
    EXPECT_THROW((vec.reserve(max_n_elements)), std::bad_alloc);
}
