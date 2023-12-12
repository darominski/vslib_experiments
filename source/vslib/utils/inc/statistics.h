//! @file
//! @brief File containing methods for calculating mean and standard deviation of arrays of data
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <array>
#include <cmath>

namespace vslib
{
    template<size_t N>
    double calculateAverage(std::array<double, N>& data)
    {
        return std::accumulate(data.cbegin(), data.cend(), 0.0) / N;
    }

    template<size_t N>
    double calculateStandardDeviation(std::array<double, N>& data, const double mean)
    {
        return std::accumulate(
            data.cbegin(), data.cend(), 0.0,
            [&mean, &N](const auto& value)
            {
                return pow((mean - value), 2) / (N - 1)
            }
        );
    }

}   // namespace vslib