//! @file
//! @brief File with unit tests of PID component.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "mockRoot.hpp"
#include "pidClassic.hpp"
#include "staticJson.hpp"

using namespace vslib;

class PIDClassicTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        ParameterRegistry& parameter_registry = ParameterRegistry::instance();
        parameter_registry.clearRegistry();
    }

    void set_pid_parameters(
        PIDClassic& pid, double p, double i, double d, double ff, double b, double c = 1, double max_integral = 1000
    )
    {
        StaticJson p_value = p;
        pid.kp.setJsonValue(p_value);

        StaticJson i_value = i;
        pid.ki.setJsonValue(i_value);

        StaticJson d_value = d;
        pid.kd.setJsonValue(d_value);

        StaticJson ff_value = ff;
        pid.kff.setJsonValue(ff_value);

        StaticJson b_value = b;
        pid.b.setJsonValue(b_value);

        StaticJson c_value = c;
        pid.c.setJsonValue(c_value);

        StaticJson integral_limit_value = max_integral;
        pid.integral_limit.setJsonValue(integral_limit_value);
        pid.flipBufferState();
        pid.synchroniseParameterBuffers();
    }
};

//! Checks that a default PID object can be constructed and is correctly added to the registry
TEST_F(PIDClassicTest, PIDClassicDefaultConstruction)
{
    MockRoot    root;
    std::string name = "pid_1";
    PIDClassic  pid(name, root);
    EXPECT_EQ(pid.getName(), name);
    EXPECT_EQ(pid.getError(), 0.0);
    EXPECT_EQ(pid.getPreviousError(), 0.0);
    EXPECT_EQ(pid.getStartingValue(), 0.0);
    EXPECT_EQ(pid.getIntegral(), 0.0);

    auto serialized_pid = pid.serialize();
    EXPECT_EQ(serialized_pid["name"], name);
    EXPECT_EQ(serialized_pid["type"], "PIDClassic");
    EXPECT_EQ(serialized_pid["components"], nlohmann::json::array());
    EXPECT_EQ(serialized_pid["parameters"].size(), 7);
    EXPECT_EQ(serialized_pid["parameters"][0]["name"], "kp");
    EXPECT_EQ(serialized_pid["parameters"][1]["name"], "ki");
    EXPECT_EQ(serialized_pid["parameters"][2]["name"], "kd");
    EXPECT_EQ(serialized_pid["parameters"][3]["name"], "kff");
    EXPECT_EQ(serialized_pid["parameters"][4]["name"], "b");
    EXPECT_EQ(serialized_pid["parameters"][5]["name"], "c");
    EXPECT_EQ(serialized_pid["parameters"][6]["name"], "integral_limit");
}

//! Checks that a PID object with an anti-windup function defined can be constructed
TEST_F(PIDClassicTest, PIDClassicAntiWindupConstruction)
{
    MockRoot     root;
    std::string  name                 = "pid_2";
    double const max_integral         = 1500;
    auto         anti_windup_function = [&max_integral](double input)
    {
        return input > max_integral ? max_integral : input;
    };   // clamping anti-windup
    PIDClassic pid(name, root, anti_windup_function);
    EXPECT_EQ(pid.getName(), name);
    EXPECT_EQ(pid.getError(), 0.0);
    EXPECT_EQ(pid.getPreviousError(), 0.0);
    EXPECT_EQ(pid.getStartingValue(), 0.0);
    EXPECT_EQ(pid.getIntegral(), 0.0);
}

//! Checks that target setter interact correctly with PID object
TEST_F(PIDClassicTest, PIDClassicSetters)
{
    MockRoot    root;
    std::string name = "pid_3";
    PIDClassic  pid(name, root);

    const double starting_value = 2 * 3.14159;
    pid.setStartingValue(starting_value);
    EXPECT_EQ(pid.getStartingValue(), starting_value);
}

//! Checks that reset method correctly sets all internal parameters to zero, and sets new starting value
TEST_F(PIDClassicTest, PIDClassicReset)
{
    MockRoot    root;
    std::string name = "pid_4";
    PIDClassic  pid(name, root);

    const double starting_value = 2 * 3.14159;
    pid.setStartingValue(starting_value);
    EXPECT_EQ(pid.getStartingValue(), starting_value);

    const double new_starting_value = 1.0;
    pid.reset(new_starting_value);
    EXPECT_EQ(pid.getStartingValue(), new_starting_value);
}

//! Checks that single iteration of control method correctly calculates the gain
TEST_F(PIDClassicTest, PIDClassicSingleIteration)
{
    MockRoot     root;
    std::string  name = "pid_5";
    PIDClassic   pid(name, root);
    const double p  = 2.0;
    const double i  = 1.0;
    const double d  = 1.5;
    const double ff = 0.05;
    const double b  = 1.2;
    const double c  = 0.5;
    set_pid_parameters(pid, p, i, d, ff, b, c);

    const double target_value = 3.14159;

    const double starting_value = 1.0;
    pid.setStartingValue(starting_value);

    const double error          = target_value - starting_value;
    const double expected_value = (target_value * b - starting_value) * p + error * i + d * error + target_value * ff;
    EXPECT_NEAR(pid.control(target_value, starting_value), expected_value, 1e-6);
}

//! Checks that a couple of iterations of control method correctly calculates gains
TEST_F(PIDClassicTest, PIDClassicControlIteration)
{
    MockRoot     root;
    std::string  name = "pid_6";
    PIDClassic   pid(name, root);
    const double p  = 0.6;
    const double i  = 0.3;
    const double d  = 0.06;
    const double ff = 0.03;
    const double b  = 1.11;
    set_pid_parameters(pid, p, i, d, ff, b);

    const double target_value = 3.14159;

    const double starting_value = 0.0;
    pid.setStartingValue(starting_value);

    double       current_error = target_value - starting_value;
    double       derivative    = -0.2 * 0.0 + d * (current_error + 0.8 * 0);
    const double first_actuation
        = (target_value * b - starting_value) * p + current_error * i + derivative + target_value * ff;
    EXPECT_NEAR(pid.control(target_value, starting_value), first_actuation, 1e-6);

    double previous_error = current_error;
    current_error         = target_value - first_actuation;
    derivative            = -0.2 * derivative + d * (current_error + 0.8 * previous_error);

    const double second_actuation = (target_value * b - first_actuation) * p + (2 * target_value - first_actuation) * i
                                    + derivative + target_value * ff;


    EXPECT_NEAR(pid.control(target_value, first_actuation), second_actuation, 1e-6);

    previous_error = current_error;
    current_error  = target_value - second_actuation;
    derivative     = -0.2 * derivative + d * (current_error + 0.8 * previous_error);

    const double third_actuation = (target_value * b - second_actuation) * p
                                   + (3 * target_value - first_actuation - second_actuation) * i + derivative
                                   + target_value * ff;
    EXPECT_NEAR(pid.control(target_value, second_actuation), third_actuation, 1e-6);
}
