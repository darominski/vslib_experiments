//! @file
//! @brief File with unit tests of LogString class.
//! @author Dominik Arominski

#include <gtest/gtest.h>
#include <string>

#include "constants.h"
#include "logString.h"

using namespace fgc4::utils;

class LogStringTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks that basic empty string works for LogString
TEST(LogStringTest, BasicEmptyConstruction)
{
    LogString str;
    EXPECT_EQ(str, "");
}

//! Checks that basic string creation from literal works for LogString
TEST(LogStringTest, BasicConstruction)
{
    LogString str("Hello world!");
    EXPECT_EQ(str, "Hello world!");
}

//! Checks that basic string creation from std::string works for LogString
TEST(LogStringTest, BasicConstructionFromString)
{
    LogString str(std::string("Hello world!"));
    EXPECT_EQ(str, "Hello world!");
}

//! Checks that the correct allocator is used
TEST(LogStringTest, AllocatorUsage)
{
    LogString  str("Hello world!");
    const auto allocator = str.get_allocator();
    EXPECT_TRUE((std::is_base_of_v<decltype(allocator), StringAllocator>));
}

//! Checks that copy-assign construction works for LogString
TEST(LogStringTest, CopyAssignConstruction)
{
    LogString log1("Hello");
    LogString log2 = log1;   // Tests copy-assign constructor
    EXPECT_EQ(log1, log2);
}

//! Checks that operator+= works for LogString
TEST(LogStringTest, OperatorPlusEqual)
{
    LogString str("Hello, ");
    str += "world!";
    EXPECT_EQ(str, "Hello, world!");
}

//! Checks that operator== works for LogString
TEST(LogStringTest, ComparisonOperator)
{
    LogString str1("Hello!");
    LogString str2("Hello!");
    EXPECT_TRUE(str1 == str2);
}

//! Checks that operator!= works for LogString
TEST(LogStringTest, NonEqualityComparisonOperator)
{
    LogString str1("Hello!");
    LogString str2("different");
    EXPECT_TRUE(str1 != str2);
}

//! Checks that copy operator works for LogString
TEST(LogStringTest, CopyOperator)
{
    LogString log1("Hello");
    LogString log2;
    log2 = log1;   // Copy operator
    EXPECT_EQ(log1, log2);
}

//! Checks that LogString resize works
TEST(LogStringTest, StringResize)
{
    LogString str("Hello");
    str.resize(10, '!');
    EXPECT_EQ(str, "Hello!!!!!");
}

//! Checks that a large string just within limits can be allocated
TEST(LogStringTest, LongStringTest)
{
    const std::size_t max_length = constants::string_memory_pool_size / 2 - 1;
    LogString         str(max_length, 'a');
    EXPECT_EQ(str.length(), max_length);
}

//! Checks that the expected exception is thrown when too large string is allocated
TEST(LogStringTest, AllocationError)
{
    const std::size_t max_length = constants::string_memory_pool_size / 2;
    EXPECT_THROW(LogString(max_length + 1, 'a'), std::length_error);
}
