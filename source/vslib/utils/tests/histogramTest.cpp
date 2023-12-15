//! @file
//! @brief Tests for simple histogramming class.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "histogram.h"

using namespace vslib::utils;

class HistogramTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HistogramTest, CreateEmptyHistogram)
{
    constexpr int number_bins = 24;
    double const  min         = -10;
    double const  max         = 17.5;
    double const  bin_width   = (max - min) / number_bins;

    Histogram<number_bins> histogram(min, max);
    ASSERT_EQ(histogram.getBinNumber(), number_bins);
    EXPECT_EQ(histogram.getBinWidth(), bin_width);
    // check that bin edges are calculated correctly
    for (int index = 0; index <= number_bins; index++)
    {
        auto const edges = histogram.getBinEdges(index);
        EXPECT_EQ(edges.first, min + index * bin_width);
    }
    // check that data is initialized correctly
    auto const data = histogram.getData();
    for (auto const value : data)
    {
        EXPECT_EQ(value, 0.0);
    }
}

TEST_F(HistogramTest, FillHistogram)
{
    constexpr int number_bins = 10;
    double const  min         = -10;
    double const  max         = 20;
    double const  bin_width   = (max - min) / number_bins;

    Histogram<number_bins> histogram(min, max);
    // tests underflow
    double underflow = min - 1;
    histogram.addValue(underflow);
    // tests overflow
    double overflow = max * 2;
    histogram.addValue(overflow);
    // tests regular case
    double value = 0;
    histogram.addValue(value);
    // add twice to create a bin with the highest number of counts:
    histogram.addValue(value);

    // check that data was placed correctly
    auto const data = histogram.getData();
    EXPECT_EQ(data[0], 1);                 // underflow
    EXPECT_EQ(data[number_bins - 1], 1);   // overflow
    int expected_bin_number = (value - min) / bin_width;
    EXPECT_EQ(data[expected_bin_number], 2);   // regular case
}

TEST_F(HistogramTest, MaximumFinder)
{
    constexpr int number_bins = 10;
    double const  min         = -10;
    double const  max         = 20;
    double const  bin_width   = (max - min) / number_bins;

    Histogram<number_bins> histogram(min, max);
    double const           underflow = min - 1;
    histogram.addValue(underflow);
    double const overflow = max * 2;
    histogram.addValue(overflow);
    double const value = 0;
    histogram.addValue(value);
    histogram.addValue(value);

    int const expected_bin_number = (value - min) / bin_width;
    // check that the maximum is at the expected location
    int max_counts_bin            = histogram.getBinWithMax();
    EXPECT_EQ(expected_bin_number, max_counts_bin);
}