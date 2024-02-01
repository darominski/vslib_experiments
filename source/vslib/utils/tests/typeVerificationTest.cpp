//! @file
//! @brief Tests for JSON and Parameter type-consistency verification methods.
//! @author Dominik Arominski

#include <array>
#include <cmath>
#include <gtest/gtest.h>

#include "staticJson.h"
#include "typeVerification.h"

using namespace vslib::utils;
using namespace fgc4::utils;

class TypeVerificationTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks that unsignedCheck correctly identifies all supported unsigned scalar types
TEST_F(TypeVerificationTest, TestUnsignedCheckUnsignedScalar)
{
    uint8_t    uint8_t_value = 2;
    StaticJson value         = uint8_t_value;
    ASSERT_TRUE(checkIfUnsigned<uint8_t>(value));

    uint16_t uint16_t_value = 2;
    value                   = uint16_t_value;
    ASSERT_TRUE(checkIfUnsigned<uint16_t>(value));

    uint32_t uint32_t_value = 123;
    value                   = uint32_t_value;
    ASSERT_TRUE(checkIfUnsigned<uint32_t>(value));

    uint64_t uint64_t_value = 123;
    value                   = uint64_t_value;
    ASSERT_TRUE(checkIfUnsigned<uint64_t>(value));
}

//! Checks that unsignedCheck correctly identifies all supported unsigned array types
TEST_F(TypeVerificationTest, TestUnsignedCheckUnsignedArray)
{
    std::array<uint8_t, 3> uint8_t_value{2};
    StaticJson             value = uint8_t_value;
    ASSERT_TRUE((checkIfUnsigned<std::array<uint8_t, 2>>(value)));

    std::array<uint16_t, 4> uint16_t_value{1};
    value = uint16_t_value;
    ASSERT_TRUE((checkIfUnsigned<std::array<uint16_t, 1>>(value)));

    std::array<uint32_t, 5> uint32_t_value{123};
    value = uint32_t_value;
    ASSERT_TRUE((checkIfUnsigned<std::array<uint32_t, 3>>(value)));

    std::array<uint64_t, 10> uint64_t_value{1241};
    value = uint64_t_value;
    ASSERT_TRUE((checkIfUnsigned<std::array<uint64_t, 5>>(value)));
}

//! Checks that unsignedCheck correctly identifies the non-supported scalar types
TEST_F(TypeVerificationTest, TestUnsignedCheckNotUnsignedScalar)
{
    int8_t     int8_t_value = -2;
    StaticJson value        = int8_t_value;
    ASSERT_FALSE(checkIfUnsigned<uint8_t>(value));

    int16_t int16_t_value = 25;
    value                 = int16_t_value;
    ASSERT_FALSE(checkIfUnsigned<uint16_t>(value));

    int32_t int32_t_value = 123;
    value                 = int32_t_value;
    ASSERT_FALSE(checkIfUnsigned<uint32_t>(value));

    int64_t int64_t_value = 123231;
    value                 = int64_t_value;
    ASSERT_FALSE(checkIfUnsigned<uint64_t>(value));

    float float_value = 3.14159;
    value             = float_value;
    ASSERT_FALSE(checkIfUnsigned<uint16_t>(value));

    double double_value = 3.14159;
    value               = double_value;
    ASSERT_FALSE(checkIfUnsigned<uint32_t>(value));
}

//! Checks that unsignedCheck correctly identifies the non-supported scalar types
TEST_F(TypeVerificationTest, TestUnsignedCheckNotUnsignedArray)
{
    // standard arrays of integers
    std::array<int8_t, 5> int8_t_value{-2};
    StaticJson            value = int8_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint8_t, 2>>(value)));

    std::array<int16_t, 1> int16_t_value{25};
    value = int16_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint16_t, 1>>(value)));

    std::array<int32_t, 5> int32_t_value{123};
    value = int32_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint32_t, 10>>(value)));

    std::array<int64_t, 7> int64_t_value{123231};
    value = int64_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint64_t, 2>>(value)));

    // standard arrays of floating-point types
    std::array<float, 2> float_value{3.14159};
    value = float_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint16_t, 12>>(value)));

    std::array<double, 5> double_value{3.14159};
    value = double_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint32_t, 1234>>(value)));

    // input array of a heterogenous floating-point data
    std::array<double, 5> heterogenous_float_value{3.14159};
    value    = heterogenous_float_value;
    value[2] = "string_not_float";   // overwrite
    ASSERT_FALSE((checkIfUnsigned<std::array<uint32_t, 1234>>(value)));

    // input array of a heterogenous data with everything correct except for one value
    std::array<uint32_t, 5> heterogenous_value{3};
    value    = heterogenous_value;
    value[2] = "string_not_uint";   // overwrite
    ASSERT_FALSE((checkIfUnsigned<std::array<uint32_t, 12>>(value)));
}

//! Checks that booleanCheck correctly identifies boolean type
TEST_F(TypeVerificationTest, TestBooleanCheckBoolean)
{
    bool       bool_value = true;
    StaticJson value      = bool_value;
    ASSERT_TRUE(checkIfBoolean<bool>(value));

    bool_value = false;
    value      = bool_value;
    ASSERT_TRUE(checkIfBoolean<bool>(value));
}

//! Checks that booleanCheck correctly identifies std::array holding boolean type
TEST_F(TypeVerificationTest, TestBooleanCheckBooleanArray)
{
    std::array<bool, 5> bool_value{true};
    StaticJson          value = bool_value;
    ASSERT_TRUE((checkIfBoolean<std::array<bool, 2>>(value)));
}

//! Checks that booleanCheck correctly identifies non-supported types
TEST_F(TypeVerificationTest, TestBooleanCheckNonBooleanScalar)
{
    int16_t    int16_value = true;
    StaticJson value       = int16_value;
    ASSERT_FALSE(checkIfBoolean<bool>(value));

    int32_t int32_value = false;
    value               = int32_value;
    ASSERT_FALSE(checkIfBoolean<bool>(value));

    uint16_t uint16_value = false;
    value                 = uint16_value;
    ASSERT_FALSE(checkIfBoolean<bool>(value));

    uint32_t uint32_value = false;
    value                 = uint32_value;
    ASSERT_FALSE(checkIfBoolean<bool>(value));

    float float_value = false;
    value             = float_value;
    ASSERT_FALSE(checkIfBoolean<bool>(value));

    double double_value = false;
    value               = double_value;
    ASSERT_FALSE(checkIfBoolean<bool>(value));
}


//! Checks that booleanCheck correctly identifies non-supported array types
TEST_F(TypeVerificationTest, TestBooleanCheckNonBooleanArray)
{
    std::array<int16_t, 5> int16_value{true};
    StaticJson             value = int16_value;
    ASSERT_FALSE((checkIfBoolean<std::array<bool, 4>>(value)));

    std::array<int32_t, 2> int32_value{false};
    value = int32_value;
    ASSERT_FALSE((checkIfBoolean<std::array<bool, 9>>(value)));

    std::array<uint16_t, 10> uint16_value{false};
    value = uint16_value;
    ASSERT_FALSE((checkIfBoolean<std::array<bool, 1>>(value)));

    std::array<uint32_t, 3> uint32_value{false};
    value = uint32_value;
    ASSERT_FALSE((checkIfBoolean<std::array<bool, 10>>(value)));

    std::array<float, 111> float_value{false};
    value = float_value;
    ASSERT_FALSE((checkIfBoolean<std::array<bool, 5>>(value)));

    std::array<double, 200> double_value{false};
    value = double_value;
    ASSERT_FALSE((checkIfBoolean<std::array<bool, 2>>(value)));
}

//! Checks that integralCheck correctly identifies all supported integral scalar types
TEST_F(TypeVerificationTest, TestIntegralCheckIntegralScalar)
{
    uint8_t    uint8_t_value = 2;
    StaticJson value         = uint8_t_value;
    ASSERT_TRUE(checkIfIntegral<uint8_t>(value));

    int8_t int8_t_value = 8;
    value               = uint8_t_value;
    ASSERT_TRUE(checkIfIntegral<int8_t>(value));

    uint16_t uint16_t_value = 2;
    value                   = uint16_t_value;
    ASSERT_TRUE(checkIfIntegral<uint16_t>(value));

    int16_t int16_t_value = 2;
    value                 = int16_t_value;
    ASSERT_TRUE(checkIfIntegral<int16_t>(value));

    uint32_t uint32_t_value = 123;
    value                   = uint32_t_value;
    ASSERT_TRUE(checkIfIntegral<uint32_t>(value));

    int32_t int32_t_value = 123;
    value                 = int32_t_value;
    ASSERT_TRUE(checkIfIntegral<int32_t>(value));

    uint64_t uint64_t_value = 123;
    value                   = uint64_t_value;
    ASSERT_TRUE(checkIfIntegral<uint64_t>(value));

    int64_t int64_t_value = 123;
    value                 = int64_t_value;
    ASSERT_TRUE(checkIfIntegral<int64_t>(value));
}

//! Checks that integralCheck correctly identifies all supported integral array types
TEST_F(TypeVerificationTest, TestIntegralCheckIntegralArray)
{
    std::array<uint8_t, 3> uint8_t_value{2};
    StaticJson             value = uint8_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<uint8_t, 2>>(value)));

    std::array<int8_t, 3> int8_t_value{2};
    value = int8_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<int8_t, 2>>(value)));

    std::array<uint16_t, 4> uint16_t_value{1};
    value = uint16_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<uint16_t, 1>>(value)));

    std::array<int16_t, 4> int16_t_value{1};
    value = int16_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<int16_t, 1>>(value)));

    std::array<uint32_t, 5> uint32_t_value{123};
    value = uint32_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<uint32_t, 3>>(value)));

    std::array<int32_t, 5> int32_t_value{123};
    value = int32_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<int32_t, 3>>(value)));

    std::array<uint64_t, 10> uint64_t_value{1241};
    value = uint64_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<uint64_t, 5>>(value)));

    std::array<int64_t, 10> int64_t_value{1241};
    value = int64_t_value;
    ASSERT_TRUE((checkIfIntegral<std::array<int64_t, 5>>(value)));
}

//! Checks that integralCheck correctly identifies all unsupported scalar types
TEST_F(TypeVerificationTest, TestIntegralCheckNonIntegralScalar)
{
    bool       bool_value = true;
    StaticJson value      = bool_value;
    ASSERT_FALSE(checkIfIntegral<int8_t>(value));

    float float_value = 8;
    value             = float_value;
    ASSERT_FALSE(checkIfIntegral<int8_t>(value));

    double double_value = 2;
    value               = double_value;
    ASSERT_FALSE(checkIfIntegral<int16_t>(value));
}

//! Checks that integralCheck correctly identifies all unsupported array types
TEST_F(TypeVerificationTest, TestIntegralCheckNonIntegralArray)
{
    std::array<bool, 2> bool_value{true};
    StaticJson          value = bool_value;
    ASSERT_FALSE((checkIfIntegral<std::array<int8_t, 5>>(value)));

    std::array<float, 3> float_value{8};
    value = float_value;
    ASSERT_FALSE((checkIfIntegral<std::array<int8_t, 1>>(value)));

    std::array<double, 5> double_value{2};
    value = double_value;
    ASSERT_FALSE((checkIfIntegral<std::array<int16_t, 2>>(value)));

    std::array<int, 5> heterogenous_value{2};
    value    = heterogenous_value;
    value[1] = "string_not_int";
    ASSERT_FALSE((checkIfIntegral<std::array<int16_t, 2>>(value)));
}