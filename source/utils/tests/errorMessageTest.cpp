//! @file
//! @brief File with unit tests of Error class.
//! @author Dominik Arominski

#include <gtest/gtest.h>
#include <string>

#include "errorMessage.h"

using namespace fgc4::utils;

class ErrorMessageTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST(ErrorMessageTest, BasicErrorStructure)
{
    Error error("Test error message", 42);

    // Verify the error message and code
    EXPECT_EQ(error.error_str, "Test error message");
    EXPECT_EQ(error.error_code, 42U);
}

TEST(ErrorMessageTest, ErrorMessageFormatting)
{
    Error error("Test error message", 42);

    // Format the error message
    std::string formatted_error = fmt::format("{}", error);

    // Verify the formatted error message
    EXPECT_EQ(formatted_error, "Error [42]: Test error message");
}

TEST(ErrorMessageTest, ErrorMessageFormattingWithoutCode)
{
    Error error("Test error message");

    // Format the error message
    std::string formatted_error = fmt::format("{}", error);

    // Verify the formatted error message (without code)
    EXPECT_EQ(formatted_error, "Error [0]: Test error message");
}
