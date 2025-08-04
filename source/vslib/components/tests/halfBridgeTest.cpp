//! @file
//! @brief File with unit tests for HalfBridge component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "halfBridge.hpp"
#include "mockRoot.hpp"

using namespace vslib;

class HalfBridgeTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Tests default construction of HalfBridge component
TEST_F(HalfBridgeTest, Construction)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);
    ASSERT_EQ(pwm.getName(), "pwm_1");

    auto serialized = pwm.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "HalfBridge");
    EXPECT_EQ(serialized["components"], nlohmann::json::array());
    EXPECT_EQ(serialized["parameters"].size(), 0);
}

//! Tests starting the HalfBridge
TEST_F(HalfBridgeTest, Start)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.start();
    // check status
}

//! Tests starting and then stopping the HalfBridge
TEST_F(HalfBridgeTest, StartStop)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.start();
    pwm.stop();
    // check status
}

//! Tests setting the PWMA output to be enabled
TEST_F(HalfBridgeTest, SetEnabledATrue)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setEnabledA(true);
    // check status
}

//! Tests setting the PWMB output to be enabled
TEST_F(HalfBridgeTest, SetEnabledBTrue)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setEnabledB(true);
    // check status
}

//! Tests setting the PWMA output to be disabled
TEST_F(HalfBridgeTest, SetEnabledAFalse)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setEnabledA(false);
    // check status
}

//! Tests setting the PWMB output to be disabled
TEST_F(HalfBridgeTest, SetEnabledBFalse)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setEnabledB(false);
    // check status
}

//! Tests setting a standard modulation index
TEST_F(HalfBridgeTest, SetModulationIndex)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setModulationIndex(0.5);
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexUndeflow)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setModulationIndex(-1.1);
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexUndeflowInf)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setModulationIndex(-std::numeric_limits<float>::infinity());
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexOverflow)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setModulationIndex(1.1);
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexOverflowInf)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setModulationIndex(std::numeric_limits<float>::infinity());
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexNaN)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setModulationIndex(std::numeric_limits<float>::quiet_NaN());
    // check status
}

//! Tests setting PWM output to high
TEST_F(HalfBridgeTest, SetHigh)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setHigh();
    // check status
}

//! Tests setting PWM output to low
TEST_F(HalfBridgeTest, SetLow)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setLow();
    // check status
}

//! Tests setting additional dead time
TEST_F(HalfBridgeTest, SetAdditionalDeadTime)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setAdditionalDeadTime(10);   // 10 extra time cycles
    // check status
}


//! Tests setting the update type to immediate
TEST_F(HalfBridgeTest, SetUpdateTypeImmediate)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setUpdateType(hal::PWM<0>::UpdateType::immediate);
    // check status
}

//! Tests setting PWM update type to bottom (shadow)
TEST_F(HalfBridgeTest, SetUpdateTypeBottom)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setUpdateType(hal::PWM<0>::UpdateType::zero);
    // check status
}

//! Tests setting PWM update type to top (shadow)
TEST_F(HalfBridgeTest, SetUpdateTypeTop)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setUpdateType(hal::PWM<0>::UpdateType::period);
    // check status
}

//! Tests setting PWM update type to bottom & top (shadow)
TEST_F(HalfBridgeTest, SetUpdateTypeBottomTop)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setUpdateType(hal::PWM<0>::UpdateType::zeroPeriod);
    // check status
}

//! Tests setting the PWM A&B output to be inverted
TEST_F(HalfBridgeTest, SetInverted)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setInverted(true);
    // check status
}

//! Tests setting the PWM A&B output to not be inverted
TEST_F(HalfBridgeTest, SetInvertedFalse)
{
    MockRoot         root;
    std::string_view name = "pwm_1";
    HalfBridge<0>    pwm(name, root);

    pwm.setInverted(true);
    pwm.setInverted(false);
    // check status
}