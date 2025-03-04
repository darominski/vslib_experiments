//! @file
//! @brief Tests for container search methods.
//! @author Dominik Arominski

#include <gtest/gtest.h>

#include "containerSearch.hpp"

using namespace vslib::utils;

class ContainerSearchTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

template<typename IndexType, typename StoredType>
std::vector<std::pair<IndexType, StoredType>> createContainer(size_t number_points, IndexType min, IndexType max)
{
    std::vector<std::pair<IndexType, StoredType>> vector(number_points);
    for (size_t index = 0; index < number_points; index++)
    {
        IndexType  x  = min + index * (max - min) / number_points;
        StoredType y  = 2 * x;
        vector[index] = std::make_pair(x, y);
    }
    return vector;
}

TEST_F(ContainerSearchTest, IndexSearchInt)
{
    const int    min           = -5;
    const int    max           = 5;
    const size_t number_points = 10;
    const int    bin_size      = (max - min) / number_points;
    auto         container     = createContainer<int, int>(number_points, min, max);
    int          x1, x2, y1, y2;

    indexSearch<int, int>(container, min, min, bin_size, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    indexSearch<int, int>(container, 0, min, bin_size, x1, y1, x2, y2);
    EXPECT_NEAR(x1, 0, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, 0, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, bin_size * 2, static_cast<double>(bin_size) / 2.0);

    indexSearch<int, int>(container, max - bin_size, min, bin_size, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, IndexSearchIntOutsideBounds)
{
    const int    min           = -5;
    const int    max           = 5;
    const size_t number_points = 10;
    const int    bin_size      = (max - min) / number_points;
    auto         container     = createContainer<int, int>(number_points, min, max);
    int          x1, x2, y1, y2;

    indexSearch<int, int>(container, -50, min, bin_size, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    indexSearch<int, int>(container, max * 2, min, bin_size, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, IndexSearchDouble)
{
    const double min           = -5;
    const double max           = 5;
    const size_t number_points = 100;
    const double bin_size      = (max - min) / number_points;
    auto         container     = createContainer<double, double>(number_points, min, max);
    double       x1, x2, y1, y2;

    indexSearch<double, double>(container, min, min, bin_size, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    indexSearch<double, double>(container, 0, min, bin_size, x1, y1, x2, y2);
    EXPECT_NEAR(x1, -bin_size, bin_size / 2.0);
    EXPECT_NEAR(y1, -bin_size * 2, bin_size / 2.0);
    EXPECT_NEAR(x2, 0, bin_size / 2.0);
    EXPECT_NEAR(y2, 0, bin_size / 2.0);

    indexSearch<double, double>(container, max - bin_size, min, bin_size, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, IndexSearchDoubleOutsideBounds)
{
    const double min           = -5;
    const double max           = 6;
    const size_t number_points = 100;
    const double bin_size      = (max - min) / number_points;
    auto         container     = createContainer<double, double>(number_points, min, max);
    double       x1, x2, y1, y2;

    indexSearch<double, double>(container, -50, min, bin_size, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    indexSearch<double, double>(container, max * 2, min, bin_size, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

// ************************************************************
// Linear search tests

TEST_F(ContainerSearchTest, LinearSearchInt)
{
    const int    min           = -5;
    const int    max           = 5;
    const size_t number_points = 10;
    const int    bin_size      = (max - min) / number_points;
    auto         container     = createContainer<int, int>(number_points, min, max);
    int          x1, x2, y1, y2;
    int          start_index = 0;

    start_index = linearSearch<int, int>(container, min, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    start_index = linearSearch<int, int>(container, 0, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, -bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, -bin_size * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, 0, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, 0, static_cast<double>(bin_size) / 2.0);

    linearSearch<int, int>(container, max - bin_size, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, LinearSearchIntOutsideBounds)
{
    const int    min           = -5;
    const int    max           = 5;
    const size_t number_points = 10;
    const int    bin_size      = (max - min) / number_points;
    auto         container     = createContainer<int, int>(number_points, min, max);
    int          x1, x2, y1, y2;
    int          start_index = 0;

    linearSearch<int, int>(container, -50, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    linearSearch<int, int>(container, max * 2, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - bin_size * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - bin_size * 2) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, LinearSearchDouble)
{
    const double min           = -5;
    const double max           = 5;
    const size_t number_points = 100;
    const double bin_size      = (max - min) / number_points;
    auto         container     = createContainer<double, double>(number_points, min, max);
    double       x1, x2, y1, y2;
    int          start_index = 0;

    start_index = linearSearch<double, double>(container, min, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    start_index = linearSearch<double, double>(container, 0, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, -bin_size, bin_size / 2.0);
    EXPECT_NEAR(y1, -bin_size * 2, bin_size / 2.0);
    EXPECT_NEAR(x2, 0, bin_size / 2.0);
    EXPECT_NEAR(y2, 0, bin_size / 2.0);

    linearSearch<double, double>(container, max - bin_size, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, LinearSearchDoubleOutsideBounds)
{
    const double min           = -5;
    const double max           = 6;
    const size_t number_points = 100;
    const double bin_size      = (max - min) / number_points;
    auto         container     = createContainer<double, double>(number_points, min, max);
    double       x1, x2, y1, y2;
    int          start_index = 0;

    linearSearch<double, double>(container, min * 2, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    linearSearch<double, double>(container, max * 2, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

// ************************************************************
// Binary search tests

TEST_F(ContainerSearchTest, BinarySearchInt)
{
    const int    min           = -5;
    const int    max           = 5;
    const size_t number_points = 10;
    const int    bin_size      = (max - min) / number_points;
    auto         container     = createContainer<int, int>(number_points, min, max);
    int          x1, x2, y1, y2;
    int          start_index = 0;

    start_index = binarySearch<int, int>(container, min, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    start_index = binarySearch<int, int>(container, 0, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, -bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, -bin_size * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, 0, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, 0, static_cast<double>(bin_size) / 2.0);

    binarySearch<int, int>(container, max - bin_size, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, BinarySearchIntOutsideBounds)
{
    const int    min           = -5;
    const int    max           = 5;
    const size_t number_points = 10;
    const int    bin_size      = (max - min) / number_points;
    auto         container     = createContainer<int, int>(number_points, min, max);
    int          x1, x2, y1, y2;
    int          start_index = 0;

    binarySearch<int, int>(container, -50, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    binarySearch<int, int>(container, max * 2, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - bin_size * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - bin_size * 2) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, BinarySearchDouble)
{
    const double min           = -5;
    const double max           = 5;
    const size_t number_points = 100;
    const double bin_size      = (max - min) / number_points;
    auto         container     = createContainer<double, double>(number_points, min, max);
    double       x1, x2, y1, y2;
    int          start_index = 0;

    start_index = binarySearch<double, double>(container, min, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    start_index = binarySearch<double, double>(container, 0, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, -bin_size, bin_size / 2.0);
    EXPECT_NEAR(y1, -bin_size * 2, bin_size / 2.0);
    EXPECT_NEAR(x2, 0, bin_size / 2.0);
    EXPECT_NEAR(y2, 0, bin_size / 2.0);

    binarySearch<double, double>(container, max - bin_size, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}

TEST_F(ContainerSearchTest, BinarySearchDoubleOutsideBounds)
{
    const double min           = -5;
    const double max           = 6;
    const size_t number_points = 100;
    const double bin_size      = (max - min) / number_points;
    auto         container     = createContainer<double, double>(number_points, min, max);
    double       x1, x2, y1, y2;
    int          start_index = 0;

    binarySearch<double, double>(container, min * 2, start_index, x1, y1, x2, y2);
    EXPECT_EQ(x1, min);
    EXPECT_EQ(y1, min * 2);
    EXPECT_EQ(x2, min + bin_size);
    EXPECT_EQ(y2, (min + bin_size) * 2);

    binarySearch<double, double>(container, max * 2, start_index, x1, y1, x2, y2);
    EXPECT_NEAR(x1, max - 2 * bin_size, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y1, (max - 2 * bin_size) * 2, static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(x2, (max - bin_size), static_cast<double>(bin_size) / 2.0);
    EXPECT_NEAR(y2, (max - bin_size) * 2, static_cast<double>(bin_size) / 2.0);
}
