//! @file
//! @brief File with unit tests of PLL component.
//! @author Dominik Arominski

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "pll.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;

class PLLTest : public ::testing::Test
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

    void set_parameters(
        PLL& pll, const double p, const double i, const double d, const double ff, const double b, const double c,
        const double N = 1, const double T = 1, const double f0 = 1, const double act_min = -1e9,
        const double act_max = 1e9, const double f_rated = 50, const double angle_offset = 0.0
    )
    {
        // set the PLL's pi Parameters:
        StaticJson p_value = p;
        pll.pi.kp.setJsonValue(p_value);

        StaticJson i_value = i;
        pll.pi.ki.setJsonValue(i_value);

        StaticJson d_value = d;
        pll.pi.kd.setJsonValue(d_value);

        StaticJson ff_value = ff;
        pll.pi.kff.setJsonValue(ff_value);

        StaticJson b_value = b;
        pll.pi.b.setJsonValue(b_value);

        StaticJson c_value = c;
        pll.pi.c.setJsonValue(c_value);

        StaticJson N_value = N;
        pll.pi.N.setJsonValue(N_value);

        StaticJson T_value = T;
        pll.pi.T.setJsonValue(T_value);

        StaticJson f0_value = f0;
        pll.pi.f0.setJsonValue(f0_value);

        StaticJson act_min_value = act_min;
        pll.pi.actuation_limits.min.setJsonValue(act_min_value);

        StaticJson act_max_value = act_max;
        pll.pi.actuation_limits.max.setJsonValue(act_max_value);

        StaticJson act_dead_zone = std::array<double, 2>{0.0, 0.0};
        pll.pi.actuation_limits.dead_zone.setJsonValue(act_dead_zone);

        pll.pi.actuation_limits.verifyParameters();
        pll.pi.actuation_limits.flipBufferState();
        pll.pi.actuation_limits.synchroniseParameterBuffers();

        pll.pi.verifyParameters();
        pll.pi.flipBufferState();
        pll.pi.synchroniseParameterBuffers();

        pll.angle_offset.setJsonValue(angle_offset);
        pll.f_rated.setJsonValue(f_rated);

        pll.verifyParameters();
        pll.flipBufferState();
        pll.synchroniseParameterBuffers();
    }
};


//! Checks that a PLL object can be constructed and is correctly added to the registry
TEST_F(PLLTest, PLLDefaultConstruction)
{
    RootComponent root;
    std::string   name = "pll_1";
    PLL           pll(name, root);
    EXPECT_EQ(pll.getName(), name);

    auto serialized = pll.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "PLL");

    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "f_rated");
    EXPECT_EQ(serialized["parameters"][1]["name"], "angle_offset");

    EXPECT_EQ(serialized["components"].size(), 2);   // AbcToDq0Transform and PID
    EXPECT_EQ(serialized["components"][0]["type"], "AbcToDq0Transform");
    EXPECT_EQ(serialized["components"][0]["name"], "abc_2_dq0");
    EXPECT_EQ(serialized["components"][1]["type"], "PID");
    EXPECT_EQ(serialized["components"][1]["name"], "pi");
}

//! Checks that a PLL object can calculate a single iteration of balancing
TEST_F(PLLTest, PLLOneIteration)
{
    RootComponent root;
    std::string   name = "pll_2";
    PLL           pll(name, root);
    // no need to set parameters, as the first step is always zero due to using
    // forward Euler method
    ASSERT_EQ(pll.balance(1.0, 1.0, 1.0), 0.0);
}

//! Checks that a PLL object can calculate a couple of iterations of balancing
TEST_F(PLLTest, PLLCoupleIterations)
{
    RootComponent root;
    std::string   name = "pll_3";
    PLL           pll(name, root);

    const double p            = 2.0;
    const double i            = 15.0;
    const double d            = 0.0;
    const double ff           = 0.0;
    const double b            = 1.0;
    const double c            = 1.0;
    const double N            = 1.0;
    const double T            = 1.0e-4;
    const double f0           = 1e-9;
    const double act_min      = -1e9;
    const double act_max      = 1e9;
    const double f_rated      = 50.0;
    const double angle_offset = 0.0;
    set_parameters(pll, p, i, d, ff, b, c, N, T, f0, act_min, act_max, f_rated, angle_offset);

    const double f_rated_2pi = f_rated * std::numbers::pi * 2.0;

    // the first step is always angle_offset due to using forward Euler method
    ASSERT_EQ(pll.balance(1.0, 1.0, 1.0), 0.0);
    ASSERT_EQ(pll.balance(1.0, 1.0, 1.0), T * f_rated_2pi);   // the current is balanced, so q = 0
    ASSERT_EQ(pll.balance(1.0, 1.0, 1.0), 2.0 * T * f_rated_2pi);
}

//! Checks that a PLL object can calculate a couple of iterations of balancing
TEST_F(PLLTest, PLLCoupleIterationsNonZeroOffset)
{
    RootComponent root;
    std::string   name = "pll_3";
    PLL           pll(name, root);

    const double p            = 2.0;
    const double i            = 15.0;
    const double d            = 0.0;
    const double ff           = 0.0;
    const double b            = 1.0;
    const double c            = 1.0;
    const double N            = 1.0;
    const double T            = 1.0e-4;
    const double f0           = 1e-9;
    const double act_min      = -1e9;
    const double act_max      = 1e9;
    const double f_rated      = 50.0;
    const double angle_offset = std::numbers::pi / 6.0;
    set_parameters(pll, p, i, d, ff, b, c, N, T, f0, act_min, act_max, f_rated, angle_offset);

    const double f_rated_2pi = f_rated * std::numbers::pi * 2.0;

    // the first step is always angle_offset due to using forward Euler method
    ASSERT_EQ(pll.balance(1.0, 1.0, 1.0), angle_offset);
    ASSERT_EQ(pll.balance(1.0, 1.0, 1.0), T * f_rated_2pi + angle_offset);   // the current is balanced, so q = 0
    ASSERT_EQ(pll.balance(1.0, 1.0, 1.0), 2.0 * T * f_rated_2pi + angle_offset);
}

//! Checks that the response of the PLL agrees with a Simulink model over a long simulation,
//! which includes introduced glitches
TEST_F(PLLTest, PLLSimulinkSimpleConsistency)
{
    RootComponent root;
    std::string   name = "pll_4";
    PLL           pll(name, root);

    const double p  = 50.0;
    const double i  = 200.0;
    const double d  = 0.0;
    const double ff = 0.0;
    const double b  = 1.0;
    const double c  = 1.0;
    const double N  = 1.0;
    const double T  = 1.0e-4;
    const double f0 = 1e-9;
    set_parameters(pll, p, i, d, ff, b, c, N, T, f0);

    std::filesystem::path abc_path       = "components/inputs/abc_pll.csv";
    std::filesystem::path matlab_wt_path = "components/inputs/wt_pll_kp=50_ki=200.csv";

    std::ifstream abc_file(abc_path);
    std::ifstream matlab_wt_file(matlab_wt_path);

    ASSERT_TRUE(abc_file.is_open());
    ASSERT_TRUE(matlab_wt_file.is_open());

    std::string abc_str;
    std::string matlab_wt_str;

    // getline(output_file, output_str);
    int counter = 0;
    while (getline(abc_file, abc_str) && getline(matlab_wt_file, matlab_wt_str))
    {
        std::stringstream ss(abc_str);
        std::string       a_str, b_str, c_str;

        // Get the a value
        std::getline(ss, a_str, ',');
        const auto a = std::stod(a_str);

        // Get the b value
        std::getline(ss, b_str, ',');
        const auto b = std::stod(b_str);

        // Get the c value
        std::getline(ss, c_str, ',');
        const auto c = std::stod(c_str);

        auto const matlab_wt = std::stod(matlab_wt_str);   // Matlab output
        auto const wt        = pll.balance(a, b, c);
        double     relative;
        if (matlab_wt != 0)
        {
            relative = (matlab_wt - wt) / matlab_wt;
        }
        else
        {
            relative = (matlab_wt - wt);
        }
        ASSERT_NEAR(relative, 0.0, 1e-6);   // at least 1e-6 relative precision
    }
    abc_file.close();
    matlab_wt_file.close();
}
