//! @file
//! @brief Unit tests for ReadCSV class.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>
#include <string>

#include "readCsv.hpp"

using namespace fgc4::utils::test;

class ReadCSVTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

//! Checks the basic construction of the reader with path to an existing file
TEST(ReadCSVTest, ExistingFileConstruction)
{
    ASSERT_NO_THROW(ReadCSV<3> reader("./inputs/input1.csv"));
}

//! Checks the basic construction of the reader with path to a non-existing file
TEST(ReadCSVTest, NonExistingFileConstruction)
{
    ASSERT_THROW(ReadCSV<3> reader("./inputs/non-existent.csv"), std::runtime_error);
}

//! Checks the reading of all values from the first line of the existing file
TEST(ReadCSVTest, ReadOneLine)
{
    ReadCSV<3> reader("./inputs/input1.csv");
    ASSERT_FALSE(reader.eof());
    const auto output = reader.readLine();
    ASSERT_TRUE(output.has_value());
    const auto [val1, val2, val3] = output.value();
    ASSERT_EQ(val1, 0.5);
    ASSERT_EQ(val2, 0.6);
    ASSERT_EQ(val3, 0.7);
    ASSERT_TRUE(reader.eof());
}

//! Checks the reading of all values from the first line of the existing file with a non-default separator
TEST(ReadCSVTest, ReadOneLineSemicololns)
{
    ReadCSV<3> reader("./inputs/input1_semicolons.csv", ';');
    ASSERT_FALSE(reader.eof());
    const auto output = reader.readLine();
    ASSERT_TRUE(output.has_value());
    const auto [val1, val2, val3] = output.value();
    ASSERT_EQ(val1, 0.5);
    ASSERT_EQ(val2, 0.6);
    ASSERT_EQ(val3, 0.7);
    ASSERT_TRUE(reader.eof());
}

//! Checks that an attempt to read more than the number of lines of the file fails gracefully
TEST(ReadCSVTest, AttemptReadTooManyLines)
{
    ReadCSV<3> reader("./inputs/input1.csv");
    ASSERT_FALSE(reader.eof());
    const auto output1 = reader.readLine();
    ASSERT_TRUE(output1.has_value());
    ASSERT_TRUE(reader.eof());
    const auto output2 = reader.readLine();
    ASSERT_FALSE(output2.has_value());
}

//! Checks the reading of all values from the entire file in a loop of the existing file
TEST(ReadCSVTest, ReadAllLines)
{
    ReadCSV<3> reader("./inputs/input2.csv");
    int        counter = 0;
    while (!reader.eof())
    {
        const auto output = reader.readLine();
        if (output)
        {
            const auto [val1, val2, val3] = output.value();
            ASSERT_EQ(val1, counter + 0.5);
            ASSERT_EQ(val2, counter + 0.6);
            ASSERT_EQ(val3, counter + 0.7);
            counter++;
        }
    }
    ASSERT_TRUE(reader.eof());
}

//! Checks the reading of all values from the entire file in a loop of the existing file with a non-default separator
TEST(ReadCSVTest, ReadAllLinesSemicolons)
{
    ReadCSV<3> reader("./inputs/input2_semicolons.csv", ';');
    int        counter = 0;
    while (!reader.eof())
    {
        const auto output = reader.readLine();
        if (output)
        {
            const auto [val1, val2, val3] = output.value();
            ASSERT_EQ(val1, counter + 0.5);
            ASSERT_EQ(val2, counter + 0.6);
            ASSERT_EQ(val3, counter + 0.7);
            counter++;
        }
    }
    ASSERT_TRUE(reader.eof());
}

//! Checks the reading of all rows but fewer columns than data stores works as expected
TEST(ReadCSVTest, ReadAllLinesFewerColumns)
{
    ReadCSV<2> reader("./inputs/input2.csv");
    int        counter = 0;
    while (!reader.eof())
    {
        const auto output = reader.readLine();
        if (output)
        {
            const auto [val1, val2] = output.value();
            ASSERT_EQ(val1, counter + 0.5);
            ASSERT_EQ(val2, counter + 0.6);
            counter++;
        }
    }
    ASSERT_TRUE(reader.eof());
}

//! Checks that attempting to read too many columns results in a std::runtime_error
TEST(ReadCSVTest, ReadAllLinesTooManyColumns)
{
    ReadCSV<4> reader("./inputs/input2.csv");
    ASSERT_THROW(reader.readLine(), std::runtime_error);
}

//! Checks that an attempt to read values with non-double values raises the correct exception
TEST(ReadCSVTest, ReadEmptyFile)
{
    ReadCSV<3> reader("./inputs/input3.csv");
    const auto output1 = reader.readLine();
    ASSERT_FALSE(output1.has_value());
}

//! Checks that an attempt to read values with non-numerical values raises a std::runtime_error
TEST(ReadCSVTest, ReadNonNumericData)
{
    ReadCSV<3> reader("./inputs/input4.csv");
    ASSERT_FALSE(reader.eof());
    ASSERT_NO_THROW(reader.readLine());   // the first line is correct
    ASSERT_THROW(reader.readLine(), std::runtime_error);
}

//! Checks the reading of all values from the entire file in a loop of the existing file with a non-default separator
//! (space) and a multi-line header
TEST(ReadCSVTest, ReadOneLineWithHeader)
{
    ReadCSV<3> reader("./inputs/input5.csv", ' ');
    while (!reader.eof())
    {
        const auto output = reader.readLine();
        if (output)
        {
            const auto [val1, val2, val3] = output.value();
            ASSERT_EQ(val1, 0.5);
            ASSERT_EQ(val2, 1.0);
            ASSERT_EQ(val3, 0.75);
        }
    }
    ASSERT_TRUE(reader.eof());
}

//! Checks that an attempt to read values from a file that contains only a header
TEST(ReadCSVTest, ReadFileHeaderOnly)
{
    ReadCSV<3> reader("./inputs/input6.csv");
    const auto output1 = reader.readLine();
    ASSERT_FALSE(output1.has_value());
}