//! @file
//! @brief File containing methods for calculating mean and standard deviation of arrays of data
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "typeTraits.h"

namespace vslib
{
    double calculateAverage(auto& data)
        requires fgc4::utils::Iterable<decltype(data)>
    {
        return std::accumulate(data.cbegin(), data.cend(), 0.0) / static_cast<double>(data.size());
    }

    double calculateStandardDeviation(auto& data, const double mean)
        requires fgc4::utils::Iterable<decltype(data)>
    {
        return std::accumulate(
                   data.cbegin(), data.cend(), 0.0,
                   [&mean](const auto& lhs, const auto& value)
                   {
                       return lhs + pow((mean - static_cast<double>(value)), 2);
                   }
               )
            / (static_cast<double>(data.size()) - 1);
    }

}   // namespace vslib