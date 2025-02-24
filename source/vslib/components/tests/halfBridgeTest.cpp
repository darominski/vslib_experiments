//! @file
//! @brief File with unit tests for HalfBridge component.
//! @author Dominik Arominski

#include <array>
#include <filesystem>
#include <gtest/gtest.h>

#include "halfBridge.hpp"
#include "rootComponent.hpp"

using namespace vslib;

class HalfBridgeTest : public ::testing::Test
{
  public:
    uint8_t m_buffer[vslib::HalfBridge::size() * 2];

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
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);
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
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.start();
    // check status
}

//! Tests starting and then stopping the HalfBridge
TEST_F(HalfBridgeTest, StartStop)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.start();
    pwm.stop();
    // check status
}

//! Tests setting the PWMA output to be enabled
TEST_F(HalfBridgeTest, SetEnabledATrue)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setEnabledA(true);
    // check status
}

//! Tests setting the PWMB output to be enabled
TEST_F(HalfBridgeTest, SetEnabledBTrue)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setEnabledB(true);
    // check status
}

//! Tests setting the PWMA output to be disabled
TEST_F(HalfBridgeTest, SetEnabledAFalse)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setEnabledA(false);
    // check status
}

//! Tests setting the PWMB output to be disabled
TEST_F(HalfBridgeTest, SetEnabledBFalse)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setEnabledB(false);
    // check status
}

//! Tests setting a standard modulation index
TEST_F(HalfBridgeTest, SetModulationIndex)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setModulationIndex(0.5);
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexUndeflow)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setModulationIndex(-1.1);
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexUndeflowInf)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setModulationIndex(-std::numeric_limits<float>::infinity());
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexOverflow)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setModulationIndex(1.1);
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexOverflowInf)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setModulationIndex(std::numeric_limits<float>::infinity());
    // check status
}

//! Tests setting a modulation index that should be clipped
TEST_F(HalfBridgeTest, SetModulationIndexNaN)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setModulationIndex(std::numeric_limits<float>::quiet_NaN());
    // check status
}

//! Tests setting PWM output to high
TEST_F(HalfBridgeTest, SetHigh)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setHigh();
    // check status
}

//! Tests setting PWM output to low
TEST_F(HalfBridgeTest, SetLow)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setLow();
    // check status
}

//! Tests setting additional dead time
TEST_F(HalfBridgeTest, SetAdditionalDeadTime)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setAdditionalDeadTime(10);   // 10 extra time cycles
    // check status
}


//! Tests setting the update type to immediate
TEST_F(HalfBridgeTest, SetUpdateTypeImmediate)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setUpdateType(hal::PWM::UpdateType::immediate);
    // check status
}

//! Tests setting PWM update type to bottom (shadow)
TEST_F(HalfBridgeTest, SetUpdateTypeBottom)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setUpdateType(hal::PWM::UpdateType::zero);
    // check status
}

//! Tests setting PWM update type to top (shadow)
TEST_F(HalfBridgeTest, SetUpdateTypeTop)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setUpdateType(hal::PWM::UpdateType::prd);
    // check status
}

//! Tests setting PWM update type to bottom & top (shadow)
TEST_F(HalfBridgeTest, SetUpdateTypeBottomTop)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setUpdateType(hal::PWM::UpdateType::zeroPrd);
    // check status
}

//! Tests setting the PWM A&B output to be inverted
TEST_F(HalfBridgeTest, SetInverted)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setInverted(true);
    // check status
}

//! Tests setting the PWM A&B output to not be inverted
TEST_F(HalfBridgeTest, SetInvertedFalse)
{
    RootComponent    root;
    std::string_view name = "pwm_1";
    HalfBridge       pwm(name, root, m_buffer);

    pwm.setInverted(true);
    pwm.setInverted(false);
    // check status
}