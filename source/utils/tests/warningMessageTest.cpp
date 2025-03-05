//! @file
//! @brief File with unit tests of Error class.
//! @author Dominik Arominski

#include <gtest/gtest.h>
#include <string>

#include "constants.hpp"
#include "logString.hpp"
#include "warningMessage.hpp"

using namespace fgc4::utils;

class WarningMessageTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST(WarningMessageTest, BasicWarningStructure)
{
    Warning warning("Test warning message");

    // Verify the warning message
    EXPECT_EQ(warning.warning_str, "Test warning message");
}

TEST(WarningMessageTest, WarningMessageFormatting)
{
    Warning warning("Test warning message");

    // Format the warning message
    std::string formatted_warning = fmt::format("{}", warning);

    // Verify the formatted warning message
    EXPECT_EQ(formatted_warning, "Warning: Test warning message");
}