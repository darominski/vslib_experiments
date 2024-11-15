//! @file
//! @brief Unit tests for InstantaneousPowerThreePhase Component.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "instantaneousPowerThreePhase.h"
#include "readCsv.h"
#include "rootComponent.h"
#include "staticJson.h"

using namespace vslib;

class InstantaneousPowerThreePhaseTest : public ::testing::Test
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

    void setValues(InstantaneousPowerThreePhase& component, const double p_gain, const double q_gain)
    {
        StaticJson value = p_gain;
        component.p_gain.setJsonValue(value);
        value = q_gain;
        component.q_gain.setJsonValue(value);

        component.verifyParameters();
        component.flipBufferState();
        component.p_gain.syncWriteBuffer();
        component.q_gain.syncWriteBuffer();
    }
};

//! Checks that a InstantaneousPowerThreePhase object can be constructed and is serialized
//! as expected
TEST_F(InstantaneousPowerThreePhaseTest, DefaultConstruction)
{
    RootComponent                root;
    const std::string            name = "power_transform";
    InstantaneousPowerThreePhase power(name, root);
    EXPECT_EQ(power.getName(), "power_transform");

    auto serialized = power.serialize();
    EXPECT_EQ(serialized["name"], name);
    EXPECT_EQ(serialized["type"], "InstantaneousPowerThreePhase");
    EXPECT_EQ(serialized["components"].size(), 0);
    EXPECT_EQ(serialized["components"].dump(), "[]");
    EXPECT_EQ(serialized["parameters"].size(), 2);
    EXPECT_EQ(serialized["parameters"][0]["name"], "p_gain");
    EXPECT_EQ(serialized["parameters"][0]["type"], "Float64");
    EXPECT_EQ(serialized["parameters"][1]["name"], "q_gain");
    EXPECT_EQ(serialized["parameters"][1]["type"], "Float64");
}

//! Checks that a InstantaneousPowerThreePhase object calculates single value of power as expected
TEST_F(InstantaneousPowerThreePhaseTest, SingleValue)
{
    RootComponent                root;
    const std::string            name = "power_transform";
    InstantaneousPowerThreePhase power(name, root);
    const double                 p_gain = 1.1;
    const double                 q_gain = 0.9;
    setValues(power, p_gain, q_gain);

    std::array<double, 3> v_abc{230.0, -115.0, 115.0};
    std::array<double, 3> i_abc{10.0, -5.0, -5.0};

    const auto [p, q] = power.calculate(v_abc[0], v_abc[1], v_abc[2], i_abc[0], i_abc[1], i_abc[2]);

    const auto expected_p = (v_abc[0] * i_abc[0] + v_abc[1] * i_abc[1] + v_abc[2] * i_abc[2]) * p_gain;
    const auto expected_q
        = (i_abc[0] * (v_abc[1] - v_abc[2]) + i_abc[1] * (v_abc[2] - v_abc[0]) + i_abc[2] * (v_abc[0] - v_abc[1]))
          * q_gain / sqrt(3);

    const auto relative_p = (expected_p - p) / expected_p;
    const auto relative_q = (expected_q - q) / expected_q;

    EXPECT_NEAR(relative_p, 0.0, 1e-6);
    EXPECT_NEAR(relative_q, 0.0, 1e-6);
}

//! Checks that a InstantaneousPowerThreePhase object calculates power correctly over a large number of inputs
//! and compared to Simulink model.
TEST_F(InstantaneousPowerThreePhaseTest, SimulinkConsistency)
{
    RootComponent                root;
    const std::string            name = "power_transform";
    InstantaneousPowerThreePhase power(name, root);
    const double                 p_gain = 1.0;
    const double                 q_gain = 1.0;
    setValues(power, p_gain, q_gain);

    std::array<double, 3> i_abc{0.75, 1.5, 3.5};

    std::filesystem::path v_abc_path      = "components/inputs/park_abc_sin_120degrees.csv";
    std::filesystem::path matlab_power3ph = "components/inputs/instantaneous_power_3phase.csv";

    fgc4::utils::test::ReadCSV<4> v_abc_file(v_abc_path);
    fgc4::utils::test::ReadCSV<2> matlab_power3ph_file(matlab_power3ph);

    while (!v_abc_file.eof() && !matlab_power3ph_file.eof())
    {
        const auto matlab_power3ph_line = matlab_power3ph_file.readLine();
        const auto v_abc_line           = v_abc_file.readLine();
        if (v_abc_line && matlab_power3ph_line)
        {
            auto const [time, v_a, v_b, v_c] = v_abc_line.value();
            auto const [matlab_p, matlab_q]  = matlab_power3ph_line.value();


            const auto [p, q] = power.calculate(v_a, v_b, v_c, i_abc[0], i_abc[1], i_abc[2]);
            double relative_p;
            if (matlab_p != 0)
            {
                relative_p = (matlab_p - p) / matlab_p;
            }
            else
            {
                relative_p = (matlab_p - p);
            }
            ASSERT_NEAR(relative_p, 0.0, 1e-6);   // at least 1e-6 relative precision
            double relative_q;
            if (matlab_q != 0)
            {
                relative_q = (matlab_q - q) / matlab_q;
            }
            else
            {
                relative_q = (matlab_q - q);
            }
            ASSERT_NEAR(relative_q, 0.0, 1e-6);   // at least 1e-6 relative precision
        }
    }
}
