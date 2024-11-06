//! @file
//! @brief File with unit tests of SRF PLL component.
//! @author Dominik Arominski

#include <filesystem>
#include <gtest/gtest.h>

#include "readCsv.h"
#include "rootComponent.h"
#include "srfPll.h"
#include "staticJson.h"

using namespace vslib;

class SRFPLLTest : public ::testing::Test
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
        SRFPLL& pll, const double p, const double i, const double d, const double ff, const double b, const double c,
        const double N = 1, const double T = 1, const double f0 = 1, const double act_min = -1e9,
        const double act_max = 1e9, const double f_rated = 50, const double angle_offset = 0.0
    )
    {
        // set the SRFPLL's pi Parameters:
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


//! Checks that a SRFPLL object can be constructed and is correctly added to the registry
TEST_F(SRFPLLTest, SRFPLLDefaultConstruction)
{
    RootComponent root;
    std::string   name = "pll_1";
    SRFPLL        pll(name, root);
    EXPECT_EQ(pll.getName(), name);

    auto serialized = pll.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "SRFPLL");

    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "f_rated");
    EXPECT_EQ(serialized["parameters"][1]["name"], "angle_offset");

    EXPECT_EQ(serialized["components"].size(), 2);   // AbcToDq0Transform and PID
    EXPECT_EQ(serialized["components"][0]["type"], "AbcToDq0Transform");
    EXPECT_EQ(serialized["components"][0]["name"], "abc_2_dq0");
    EXPECT_EQ(serialized["components"][1]["type"], "PID");
    EXPECT_EQ(serialized["components"][1]["name"], "pi");
}

//! Checks that a SRFPLL object can calculate a single iteration of balancing
TEST_F(SRFPLLTest, SRFPLLOneIteration)
{
    RootComponent root;
    std::string   name = "pll_2";
    SRFPLL        pll(name, root);
    // no need to set parameters, as the first step is always zero due to using
    // forward Euler method
    ASSERT_EQ(pll.synchronise(1.0, 1.0, 1.0), 0.0);
}

//! Checks that a SRFPLL object can calculate a couple of iterations of balancing
TEST_F(SRFPLLTest, SRFPLLCoupleIterations)
{
    RootComponent root;
    std::string   name = "pll_3";
    SRFPLL        pll(name, root);

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
    ASSERT_NEAR(pll.synchronise(1.0, 1.0, 1.0), 0.0, 1e-6);
    ASSERT_NEAR(pll.synchronise(1.0, 1.0, 1.0), T * f_rated_2pi, 1e-6);   // the current is balanced, so q = 0
    ASSERT_NEAR(pll.synchronise(1.0, 1.0, 1.0), 2.0 * T * f_rated_2pi, 1e-6);
}

//! Checks that a SRFPLL object can calculate a couple of iterations of balancing
TEST_F(SRFPLLTest, SRFPLLCoupleIterationsNonZeroOffset)
{
    RootComponent root;
    std::string   name = "pll_3";
    SRFPLL        pll(name, root);

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
    ASSERT_NEAR(pll.synchronise(1.0, 1.0, 1.0), angle_offset, 1e-6);
    ASSERT_NEAR(
        pll.synchronise(1.0, 1.0, 1.0), T * f_rated_2pi + angle_offset, 1e-6
    );   // the current is balanced, so q = 0
    ASSERT_NEAR(pll.synchronise(1.0, 1.0, 1.0), 2.0 * T * f_rated_2pi + angle_offset, 1e-6);
}

//! Checks that the response of the SRFPLL agrees with a Simulink model over a long simulation,
//! which includes introduced glitches
TEST_F(SRFPLLTest, SRFPLLSimulinkSimpleConsistency)
{
    RootComponent root;
    std::string   name = "pll_4";
    SRFPLL        pll(name, root);

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

    fgc4::utils::test::ReadCSV<3> abc_file(abc_path);
    fgc4::utils::test::ReadCSV<1> matlab_wt_file(matlab_wt_path);

    while (!abc_file.eof() && !matlab_wt_file.eof())
    {
        const auto matlab_wt_line = matlab_wt_file.readLine();
        const auto abc_line       = abc_file.readLine();
        if (abc_line && matlab_wt_line)
        {
            auto const [a, b, c]   = abc_line.value();
            auto const [matlab_wt] = matlab_wt_line.value();

            auto const wt = pll.synchronise(a, b, c);
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
    }
}