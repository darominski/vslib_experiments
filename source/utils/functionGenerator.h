//! @file
//! @brief Definition of function generating vector of index-value pairs with provided function and in defined range.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "typeTraits.h"

namespace fgc4::utils
{
    //! Generates a vector of index-value pairs with equally spaced binning from min to max
    //!
    //! @param function Function to generate y-axis values from x-axis values
    //! @param min Minimum x-axis  value
    //! @param max Maximum x-axis value
    //! @param number_points Length of the vector to generate
    //! @return Vector with index-value pairs following the provided function in the defined range
    template<fgc4::utils::NumericScalar IndexType, fgc4::utils::NumericScalar StoredType>
    std::vector<std::pair<IndexType, StoredType>>
    generateFunction(std::function<StoredType(IndexType)> function, IndexType min, IndexType max, size_t number_points)
    {
        assert(max > min);
        std::vector<std::pair<IndexType, StoredType>> data(number_points + 1);
        const auto                                    bin_size = (max - min) / number_points;

        std::generate(
            data.begin(), data.end(),
            [&, index = 0]() mutable
            {
                IndexType  x = min + index * bin_size;
                StoredType y = function(x);
                ++index;
                return std::make_pair(x, y);
            }
        );

        return data;
    }
}   // namepace fgc4::utils