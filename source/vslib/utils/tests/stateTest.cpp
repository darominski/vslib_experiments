//! @file
//! @brief Tests for the VSlib Finite State Machine class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "converter.h"
#include "rootComponent.h"
#include "state.h"

using namespace vslib;
using namespace vslib::utils;

class StateTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

class MockConverter : public IConverter
{
  public:
    MockConverter(RootComponent& root)
        : IConverter("mock", root)
    {
    }

    void init() override
    {
    }

    void backgroundTask() override
    {
    }
};

TEST_F(StateTest, InitializationTest)
{
    RootComponent root;
    MockConverter converter(root);

    ASSERT_NO_THROW(VSMachine state(root, converter));
}
