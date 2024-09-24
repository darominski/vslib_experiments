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
        const double N = 1, const double T = 1, const double f0 = 1, const double act_min = 0,
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
        pll.integrator.actuation_limits.min.setJsonValue(act_min_value);

        StaticJson act_max_value = act_max;
        pll.pi.actuation_limits.max.setJsonValue(act_max_value);
        pll.integrator.actuation_limits.max.setJsonValue(act_max_value);

        pll.pi.actuation_limits.verifyParameters();
        pll.pi.actuation_limits.flipBufferState();
        pll.pi.actuation_limits.synchroniseParameterBuffers();

        pll.pi.verifyParameters();
        pll.pi.flipBufferState();
        pll.pi.synchroniseParameterBuffers();

        // set the PLL's integrator Parameters:
        pll.integrator.kp.setJsonValue(0.0);

        pll.integrator.ki.setJsonValue(1.0);

        pll.integrator.kd.setJsonValue(0.0);

        pll.integrator.kff.setJsonValue(0.0);

        pll.integrator.b.setJsonValue(1.0);

        pll.integrator.c.setJsonValue(1.0);

        pll.integrator.N.setJsonValue(N_value);

        pll.integrator.T.setJsonValue(T_value);

        pll.integrator.f0.setJsonValue(f0_value);

        pll.integrator.actuation_limits.verifyParameters();
        pll.integrator.actuation_limits.flipBufferState();
        pll.integrator.actuation_limits.synchroniseParameterBuffers();

        pll.integrator.verifyParameters();
        pll.integrator.flipBufferState();
        pll.integrator.synchroniseParameterBuffers();

        pll.angle_offset.setJsonValue(angle_offset);
        pll.f_rated.setJsonValue(f_rated);

        pll.verifyParameters();
        pll.flipBufferState();
        pll.synchroniseParameterBuffers();
    }
};


//! Checks that a default PLL object can be constructed and is correctly added to the registry
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

    // EXPECT_EQ(serialized["components"][0]["parameters"]["name"], "kp");
    // EXPECT_EQ(serialized["parameters"][1]["name"], "ki");
    // EXPECT_EQ(serialized["parameters"][2]["name"], "kd");
    // EXPECT_EQ(serialized["parameters"][3]["name"], "kff");
    // EXPECT_EQ(serialized["parameters"][4]["name"], "proportional_scaling");
    // EXPECT_EQ(serialized["parameters"][5]["name"], "derivative_scaling");
    // EXPECT_EQ(serialized["parameters"][6]["name"], "derivative_filter_order");
    // EXPECT_EQ(serialized["parameters"][7]["name"], "control_period");
    // EXPECT_EQ(serialized["parameters"][8]["name"], "pre_warping_frequency");
}

//! Checks that the calculated actuation of RST is as expected against Simulink model
TEST_F(PLLTest, PLLSimulinkSimpleConsistency)
{
    RootComponent root;

    std::string  name = "pll_2";
    PLL          pll(name, root);
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

    // the input file is a measurement of B performed on 08/10/2020, shortened to the first 5000 points
    std::filesystem::path input_path  = "components/inputs/abc_pll.csv";
    std::filesystem::path output_path = "components/inputs/wt_pll_kp=50_ki=200.csv";

    std::ifstream input_file(input_path);
    std::ifstream output_file(output_path);

    ASSERT_TRUE(input_file.is_open());
    ASSERT_TRUE(output_file.is_open());

    std::string input_str;
    std::string output_str;

    // getline(output_file, output_str);
    int counter = 0;
    while (getline(input_file, input_str) && getline(output_file, output_str))
    {
        std::stringstream ss(input_str);
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

        auto const matlab_wt = std::stod(output_str);   // Matlab output
        auto const wt        = pll.balance(a, b, c);
        const auto relative  = (matlab_wt - wt);
        // output_file_test << wt << std::endl;
        std::cout << counter++ << " " << wt << " " << matlab_wt << std::endl;

        ASSERT_NEAR(relative, 0.0, 1e-4);   // at least 1e-6 relative precision
    }
    input_file.close();
    output_file.close();
}
