//! @file
//! @brief File containing methods for calculating mean and standard deviation of arrays of data
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cmath>
#include <numeric>

#include "typeTraits.h"

namespace vslib::utils
{
    double calculateAverage(auto& data)
        requires fgc4::utils::Iterable<decltype(data)>
    {
        return std::accumulate(data.cbegin(), data.cend(), 0.0) / static_cast<double>(data.size());
    }

    double calculateStandardDeviation(auto& data, const double mean)
        requires fgc4::utils::Iterable<decltype(data)>
    {
        double const square_sum = std::accumulate(
            data.cbegin(), data.cend(), 0.0,
            [&mean](const auto& lhs, const auto& value)
            {
                return lhs + pow((mean - static_cast<double>(value)), 2);
            }
        );
        return sqrt(square_sum / (static_cast<double>(data.size()) - 1));
    }

}   // namespace vslib
