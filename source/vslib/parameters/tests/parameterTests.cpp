//! @file
//! @brief  Main function for parameter namespace tests
//! @author Dominik Arominski

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}