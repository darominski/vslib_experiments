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
    ASSERT_FALSE(checkIfUnsigned<uint8_t>(value).has_value());

    uint16_t uint16_t_value = 2;
    value                   = uint16_t_value;
    ASSERT_FALSE(checkIfUnsigned<uint16_t>(value).has_value());

    uint32_t uint32_t_value = 123;
    value                   = uint32_t_value;
    ASSERT_FALSE(checkIfUnsigned<uint32_t>(value).has_value());

    uint64_t uint64_t_value = 123;
    value                   = uint64_t_value;
    ASSERT_FALSE(checkIfUnsigned<uint64_t>(value).has_value());
}

//! Checks that unsignedCheck correctly identifies all supported unsigned array types
TEST_F(TypeVerificationTest, TestUnsignedCheckUnsignedArray)
{
    std::array<uint8_t, 3> uint8_t_value{2};
    StaticJson             value = uint8_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint8_t, 2>>(value)).has_value());

    std::array<uint16_t, 4> uint16_t_value{1};
    value = uint16_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint16_t, 1>>(value)).has_value());

    std::array<uint32_t, 5> uint32_t_value{123};
    value = uint32_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint32_t, 3>>(value)).has_value());

    std::array<uint64_t, 10> uint64_t_value{1241};
    value = uint64_t_value;
    ASSERT_FALSE((checkIfUnsigned<std::array<uint64_t, 5>>(value)).has_value());
}

//! Checks that unsignedCheck correctly identifies the non-supported scalar types
TEST_F(TypeVerificationTest, TestUnsignedCheckNotUnsignedScalar)
{
    int8_t     int8_t_value = -2;
    StaticJson value        = int8_t_value;
    auto const check_int8   = checkIfUnsigned<uint8_t>(value);
    ASSERT_TRUE(check_int8.has_value());

    int16_t int16_t_value  = 25;
    value                  = int16_t_value;
    auto const check_int16 = checkIfUnsigned<uint16_t>(value);
    ASSERT_TRUE(check_int16.has_value());

    int32_t int32_t_value  = 123;
    value                  = int32_t_value;
    auto const check_int32 = checkIfUnsigned<uint32_t>(value);
    ASSERT_TRUE(check_int32.has_value());

    int64_t int64_t_value  = 123231;
    value                  = int64_t_value;
    auto const check_int64 = checkIfUnsigned<uint64_t>(value);
    ASSERT_TRUE(check_int64.has_value());

    float float_value      = 3.14159;
    value                  = float_value;
    auto const check_float = checkIfUnsigned<uint16_t>(value);
    ASSERT_TRUE(check_float.has_value());

    double double_value     = 3.14159;
    value                   = double_value;
    auto const check_double = checkIfUnsigned<uint32_t>(value);
    ASSERT_TRUE(check_double.has_value());
}

//! Checks that unsignedCheck correctly identifies the non-supported scalar types
TEST_F(TypeVerificationTest, TestUnsignedCheckNotUnsignedArray)
{
    // standard arrays of integers
    std::array<int8_t, 5> int8_t_value{-2};
    StaticJson            value = int8_t_value;

    auto const check_int8 = checkIfUnsigned<std::array<uint8_t, 2>>(value);
    ASSERT_TRUE(check_int8.has_value());

    std::array<int16_t, 1> int16_t_value{25};
    value = int16_t_value;

    auto const check_int16 = checkIfUnsigned<std::array<uint16_t, 1>>(value);
    ASSERT_TRUE(check_int16.has_value());

    std::array<int32_t, 5> int32_t_value{123};
    value = int32_t_value;

    auto const check_int32 = checkIfUnsigned<std::array<uint32_t, 10>>(value);
    ASSERT_TRUE(check_int32.has_value());

    std::array<int64_t, 7> int64_t_value{123231};
    value = int64_t_value;

    auto const check_int64 = checkIfUnsigned<std::array<uint64_t, 2>>(value);
    ASSERT_TRUE(check_int64.has_value());

    // standard arrays of floating-point types
    std::array<float, 2> float_value{3.14159};
    value = float_value;

    auto const check_float = checkIfUnsigned<std::array<uint16_t, 12>>(value);
    ASSERT_TRUE(check_float.has_value());

    std::array<double, 5> double_value{3.14159};
    value = double_value;

    auto const check_double = checkIfUnsigned<std::array<uint32_t, 1234>>(value);
    ASSERT_TRUE(check_double.has_value());

    // input array of a heterogenous floating-point data
    std::array<double, 5> heterogenous_float_value{3.14159};
    value    = heterogenous_float_value;
    value[2] = "string_not_float";

    auto const check_heterogenous_float = checkIfUnsigned<std::array<uint32_t, 1234>>(value);
    ASSERT_TRUE(check_heterogenous_float.has_value());

    // input array of a heterogenous data with everything correct except for one value
    std::array<uint32_t, 5> heterogenous_value{3};
    value    = heterogenous_value;
    value[2] = "string_not_uint";

    auto const check_heterogenous = checkIfUnsigned<std::array<uint32_t, 12>>(value);
    ASSERT_TRUE(check_heterogenous.has_value());
}

//! Checks that booleanCheck correctly identifies boolean type
TEST_F(TypeVerificationTest, TestBooleanCheckBoolean)
{
    bool       bool_value = true;
    StaticJson value      = bool_value;
    auto const check_bool = checkIfBoolean<bool>(value);
    ASSERT_FALSE(check_bool.has_value());
}

//! Checks that booleanCheck correctly identifies std::array holding boolean type
TEST_F(TypeVerificationTest, TestBooleanCheckBooleanArray)
{
    std::array<bool, 5> bool_value{true};
    StaticJson          value      = bool_value;
    auto const          check_bool = checkIfBoolean<std::array<bool, 3>>(value);
    ASSERT_FALSE(check_bool.has_value());
}

//! Checks that booleanCheck correctly identifies non-supported types
TEST_F(TypeVerificationTest, TestBooleanCheckNonBooleanScalar)
{
    int16_t    int16_value = true;
    StaticJson value       = int16_value;

    auto const check_int16 = checkIfBoolean<bool>(value);
    ASSERT_TRUE(check_int16.has_value());

    int32_t int32_value = false;
    value               = int32_value;

    auto const check_int32 = checkIfBoolean<bool>(value);
    ASSERT_TRUE(check_int32.has_value());

    uint16_t uint16_value = false;
    value                 = uint16_value;

    auto const check_uint16 = checkIfBoolean<bool>(value);
    ASSERT_TRUE(check_uint16.has_value());

    uint32_t uint32_value = false;
    value                 = uint32_value;

    auto const check_uint32 = checkIfBoolean<bool>(value);
    ASSERT_TRUE(check_uint32.has_value());

    float float_value = false;
    value             = float_value;

    auto const check_float = checkIfBoolean<bool>(value);
    ASSERT_TRUE(check_float.has_value());

    double double_value = false;
    value               = double_value;

    auto const check_double = checkIfBoolean<bool>(value);
    ASSERT_TRUE(check_double.has_value());
}

//! Checks that booleanCheck correctly identifies non-supported array types
TEST_F(TypeVerificationTest, TestBooleanCheckNonBooleanArray)
{
    std::array<int16_t, 5> int16_value{true};
    StaticJson             value = int16_value;

    auto const check_int16 = checkIfBoolean<std::array<bool, 4>>(value);
    ASSERT_TRUE(check_int16.has_value());

    std::array<int32_t, 2> int32_value{false};
    value = int32_value;

    auto const check_int32 = checkIfBoolean<std::array<bool, 9>>(value);
    ASSERT_TRUE(check_int32.has_value());

    std::array<uint16_t, 10> uint16_value{false};
    value = uint16_value;

    auto const check_uint16 = checkIfBoolean<std::array<bool, 2>>(value);
    ASSERT_TRUE(check_uint16.has_value());

    std::array<uint32_t, 3> uint32_value{true};
    value = uint32_value;

    auto const check_uint32 = checkIfBoolean<std::array<bool, 10>>(value);
    ASSERT_TRUE(check_uint32.has_value());

    std::array<float, 90> float_value{false};
    value = float_value;

    auto const check_float = checkIfBoolean<std::array<bool, 5>>(value);
    ASSERT_TRUE(check_float.has_value());

    std::array<double, 90> double_value{false};
    value = double_value;

    auto const check_double = checkIfBoolean<std::array<bool, 2>>(value);
    ASSERT_TRUE(check_double.has_value());

    double double_non_array_value{false};
    value = double_non_array_value;

    auto const check_double_non_array = checkIfBoolean<std::array<bool, 2>>(value);
    ASSERT_TRUE(check_double_non_array.has_value());
}

//! Checks that integralCheck correctly identifies all supported integral scalar types
TEST_F(TypeVerificationTest, TestIntegralCheckIntegralScalar)
{
    uint8_t    uint8_t_value = 2;
    StaticJson value         = uint8_t_value;

    auto const check_uint8 = checkIfIntegral<uint8_t>(value);
    ASSERT_FALSE(check_uint8.has_value());

    int8_t int8_t_value = 8;
    value               = uint8_t_value;

    auto const check_int8 = checkIfIntegral<int8_t>(value);
    ASSERT_FALSE(check_int8.has_value());

    uint16_t uint16_t_value = 2;
    value                   = uint16_t_value;

    auto const check_uint16 = checkIfIntegral<uint16_t>(value);
    ASSERT_FALSE(check_uint16.has_value());

    int16_t int16_t_value = 2;
    value                 = int16_t_value;

    auto const check_int16 = checkIfIntegral<int16_t>(value);
    ASSERT_FALSE(check_int16.has_value());

    uint32_t uint32_t_value = 123;
    value                   = uint32_t_value;

    auto const check_uint32 = checkIfIntegral<uint32_t>(value);
    ASSERT_FALSE(check_uint32.has_value());

    int32_t int32_t_value = 123;
    value                 = int32_t_value;

    auto const check_int32 = checkIfIntegral<int32_t>(value);
    ASSERT_FALSE(check_int32.has_value());

    uint64_t uint64_t_value = 123;
    value                   = uint64_t_value;

    auto const check_uint64 = checkIfIntegral<uint64_t>(value);
    ASSERT_FALSE(check_uint64.has_value());

    int64_t int64_t_value = 123;
    value                 = int64_t_value;

    auto const check_int64 = checkIfIntegral<int64_t>(value);
    ASSERT_FALSE(check_int64.has_value());
}

//! Checks that integralCheck correctly identifies all supported integral array types
TEST_F(TypeVerificationTest, TestIntegralCheckIntegralArray)
{
    std::array<uint8_t, 3> uint8_t_value{2};
    StaticJson             value = uint8_t_value;

    auto const check_uint8 = checkIfIntegral<std::array<uint8_t, 2>>(value);
    ASSERT_FALSE(check_uint8.has_value());

    std::array<int8_t, 3> int8_t_value{2};
    value = int8_t_value;

    auto const check_int8 = checkIfIntegral<std::array<int8_t, 2>>(value);
    ASSERT_FALSE(check_int8.has_value());

    std::array<uint16_t, 4> uint16_t_value{1};
    value = uint16_t_value;

    auto const check_uint16 = checkIfIntegral<std::array<uint16_t, 1>>(value);
    ASSERT_FALSE(check_uint16.has_value());

    std::array<int16_t, 4> int16_t_value{1};
    value = int16_t_value;

    auto const check_int16 = checkIfIntegral<std::array<int16_t, 1>>(value);
    ASSERT_FALSE(check_int16.has_value());

    std::array<uint32_t, 5> uint32_t_value{123};
    value = uint32_t_value;

    auto const check_uint32 = checkIfIntegral<std::array<uint32_t, 3>>(value);
    ASSERT_FALSE(check_uint32.has_value());

    std::array<int32_t, 5> int32_t_value{123};
    value = int32_t_value;

    auto const check_int32 = checkIfIntegral<std::array<uint16_t, 2>>(value);
    ASSERT_FALSE(check_int32.has_value());

    std::array<uint64_t, 10> uint64_t_value{1241};
    value = uint64_t_value;

    auto const check_uint64 = checkIfIntegral<std::array<uint32_t, 2>>(value);
    ASSERT_FALSE(check_uint64.has_value());

    std::array<int64_t, 10> int64_t_value{1241};
    value = int64_t_value;

    auto const check_int64 = checkIfIntegral<std::array<uint64_t, 2>>(value);
    ASSERT_FALSE(check_int64.has_value());
}

//! Checks that integralCheck correctly identifies all unsupported scalar types
TEST_F(TypeVerificationTest, TestIntegralCheckNonIntegralScalar)
{
    bool       bool_value = true;
    StaticJson value      = bool_value;

    auto const check_bool = checkIfIntegral<int8_t>(value);
    ASSERT_TRUE(check_bool.has_value());

    float float_value = 8;
    value             = float_value;

    auto const check_float = checkIfIntegral<int16_t>(value);
    ASSERT_TRUE(check_float.has_value());

    double double_value = 2;
    value               = double_value;

    auto const check_double = checkIfIntegral<int32_t>(value);
    ASSERT_TRUE(check_double.has_value());
}

//! Checks that integralCheck correctly identifies all unsupported array types
TEST_F(TypeVerificationTest, TestIntegralCheckNonIntegralArray)
{
    std::array<bool, 2> bool_value{true};
    StaticJson          value = bool_value;

    auto const check_bool = checkIfIntegral<std::array<int8_t, 5>>(value);
    ASSERT_TRUE(check_bool);

    std::array<float, 3> float_value{8};
    value = float_value;

    auto const check_float = checkIfIntegral<std::array<int8_t, 1>>(value);
    ASSERT_TRUE(check_float.has_value());

    std::array<double, 5> double_value{2};
    value = double_value;

    auto const check_double = checkIfIntegral<std::array<int16_t, 2>>(value);
    ASSERT_TRUE(check_double.has_value());

    std::array<int, 5> heterogenous_value{2};
    value    = heterogenous_value;
    value[1] = "string_not_int";

    auto const check_heterogenous = checkIfIntegral<std::array<int16_t, 2>>(value);
    ASSERT_TRUE(check_heterogenous);

    int non_array = 5;
    value         = non_array;

    auto const check_non_array = checkIfIntegral<std::array<int16_t, 2>>(value);
    ASSERT_TRUE(check_non_array);
}