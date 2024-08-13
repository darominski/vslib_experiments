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
    //! Calculates an average of the data stored in the provided container.
    //!
    //! @param data Input container
    //! @return Average value stored in the provided container.
    double calculateAverage(const auto& data)
        requires fgc4::utils::Iterable<decltype(data)>
    {
        return std::accumulate(data.cbegin(), data.cend(), 0.0) / static_cast<double>(data.size());
    }

    //! Calculates a standard deviation of the data stored in the provided container.
    //!
    //! @param data Input container
    //! @param mean Average value of the data
    //! @return Standard deviation of the data stored in the container
    double calculateStandardDeviation(const auto& data, const double mean)
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
