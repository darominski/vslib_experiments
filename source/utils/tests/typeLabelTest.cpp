//! @file
//! @brief File with typeLabel class unit tests.
//! @author Dominik Arominski

#include <gtest/gtest.h>
#include <string>

#include "constants.hpp"
#include "typeLabel.hpp"

using namespace fgc4::utils;

class TypeLabelTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};


//! Checks the bool type label
TEST(TypeLabelTest, BoolTypeLabel)
{
    EXPECT_EQ(getTypeLabel<bool>(), constants::bool_type_label);
}

//! Checks the float type label
TEST(TypeLabelTest, FloatTypeLabel)
{
    EXPECT_EQ(getTypeLabel<float>(), constants::float32_type_label);
}

//! Checks the double type label
TEST(TypeLabelTest, DoubleTypeLabel)
{
    EXPECT_EQ(getTypeLabel<double>(), constants::float64_type_label);
}

//! Checks the enumeration type label
TEST(TypeLabelTest, EnumTypeLabel)
{
    enum class TestEnum
    {
        field1,
        field2
    };
    EXPECT_EQ(getTypeLabel<TestEnum>(), constants::enum_type_label);
}

//! Checks the int64 type label
TEST(TypeLabelTest, Int64TypeLabel)
{
    EXPECT_EQ(getTypeLabel<int64_t>(), constants::int64_type_label);
}

//! Checks the uint64 type label
TEST(TypeLabelTest, UInt64TypeLabel)
{
    EXPECT_EQ(getTypeLabel<uint64_t>(), constants::uint64_type_label);
}

//! Checks the int32 type label
TEST(TypeLabelTest, Int32TypeLabel)
{
    EXPECT_EQ(getTypeLabel<int32_t>(), constants::int32_type_label);
}

//! Checks the uint32 type label
TEST(TypeLabelTest, UInt32TypeLabel)
{
    EXPECT_EQ(getTypeLabel<uint32_t>(), constants::uint32_type_label);
}

//! Checks the int16 type label
TEST(TypeLabelTest, Int16TypeLabel)
{
    EXPECT_EQ(getTypeLabel<int16_t>(), constants::int16_type_label);
}

//! Checks the uint16 type label
TEST(TypeLabelTest, UInt16TypeLabel)
{
    EXPECT_EQ(getTypeLabel<uint16_t>(), constants::uint16_type_label);
}

//! Checks the int8 type label
TEST(TypeLabelTest, Int8TypeLabel)
{
    EXPECT_EQ(getTypeLabel<int8_t>(), constants::int8_type_label);
}

//! Checks the uint8 type label
TEST(TypeLabelTest, UInt8TypeLabel)
{
    EXPECT_EQ(getTypeLabel<uint8_t>(), constants::uint8_type_label);
}

//! Checks the string type label
TEST(TypeLabelTest, StringTypeLabel)
{
    EXPECT_EQ(getTypeLabel<std::string>(), constants::string_type_label);
}

constexpr std::string_view array_type_prefix = "Array";

//! Checks the array of bool type label
TEST(TypeLabelTest, ArrayBoolTypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<bool, 1>>()), "Array" + std::string(constants::bool_type_label));
}

//! Checks the array offloat type label
TEST(TypeLabelTest, ArrayFloatTypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<float, 1>>()), "Array" + std::string(constants::float32_type_label));
}

//! Checks the array of double type label
TEST(TypeLabelTest, ArrayDoubleTypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<double, 1>>()), "Array" + std::string(constants::float64_type_label));
}

//! Checks the array of enumeration type label
TEST(TypeLabelTest, ArrayEnumTypeLabel)
{
    enum class TestEnum
    {
        field1,
        field2
    };
    EXPECT_EQ((getTypeLabel<std::array<TestEnum, 1>>()), "Array" + std::string(constants::enum_type_label));
}

//! Checks the array of int64 type label
TEST(TypeLabelTest, ArrayInt64TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<int64_t, 1>>()), "Array" + std::string(constants::int64_type_label));
}

//! Checks the array of uint64 type label
TEST(TypeLabelTest, ArrayUInt64TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<uint64_t, 1>>()), "Array" + std::string(constants::uint64_type_label));
}

//! Checks the array of int32 type label
TEST(TypeLabelTest, ArrayInt32TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<int32_t, 1>>()), "Array" + std::string(constants::int32_type_label));
}

//! Checks the array of uint32 type label
TEST(TypeLabelTest, ArrayUInt32TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<uint32_t, 1>>()), "Array" + std::string(constants::uint32_type_label));
}

//! Checks the array of int16 type label
TEST(TypeLabelTest, ArrayInt16TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<int16_t, 1>>()), "Array" + std::string(constants::int16_type_label));
}

//! Checks the array of uint16 type label
TEST(TypeLabelTest, ArrayUInt16TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<uint16_t, 1>>()), "Array" + std::string(constants::uint16_type_label));
}

//! Checks the array of int8 type label
TEST(TypeLabelTest, ArrayInt8TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<int8_t, 1>>()), "Array" + std::string(constants::int8_type_label));
}

//! Checks the array of uint8 type label
TEST(TypeLabelTest, ArrayUInt8TypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<uint8_t, 1>>()), "Array" + std::string(constants::uint8_type_label));
}

//! Checks the array of string type label
TEST(TypeLabelTest, ArrayStringTypeLabel)
{
    EXPECT_EQ((getTypeLabel<std::array<std::string, 1>>()), "Array" + std::string(constants::string_type_label));
}
