//! @file
//! @brief Defines the utilities to be used by interpolation methods to find the point corresponding to the provided
//! value.
//! @author Dominik Arominski

#pragma once

#include <algorithm>
#include <vector>

namespace vslib::utils
{
    //! Performs index-calculation "search" of the provided input_x in the values container when bins are equally
    //! spaced.
    //!
    //! @param input_x Input x-axis value to be searched for
    //! @param x1 X-axis value of the lower section edge
    //! @param y1 Y-axis value of the lower section edge
    //! @param x2 X-axis value of the upper section edge
    //! @param y2 Y-axis value of the upper section edge
    template<typename IndexType, typename StoredType = IndexType>
    inline void index_search(
        std::vector<std::pair<IndexType, StoredType>>& values, IndexType input_x, IndexType lower_edge_x,
        IndexType bin_size, IndexType& x1, StoredType& y1, IndexType& x2, StoredType& y2
    ) noexcept
    {
        int64_t position = static_cast<int64_t>(bin_size + (input_x - lower_edge_x) / bin_size);
        // limits protection, never go outside the range of the provided vector:
        if (position >= static_cast<int64_t>(values.size()))
        {
            position = values.size() - 1;
        }
        else if (position <= 0)
        {
            position = 1;
        }

        x1 = values[position - 1].first;
        y1 = values[position - 1].second;
        x2 = values[position].first;
        y2 = values[position].second;
    }

    //! Performs linear search of the provided input_x in the m_values container.
    //!
    //! @param input_x Input x-axis value to be searched for
    //! @param start_index Index to start the search from
    //! @param x1 X-axis value of the lower section edge
    //! @param y1 Y-axis value of the lower section edge
    //! @param x2 X-axis value of the upper section edge
    //! @param y2 Y-axis value of the upper section edge
    template<typename IndexType, typename StoredType = IndexType>
    inline size_t linear_search(
        std::vector<std::pair<IndexType, StoredType>>& values, IndexType input_x, size_t start_index, IndexType& x1,
        StoredType& y1, IndexType& x2, StoredType& y2
    ) noexcept
    {
        // begin() + 1 shift protects from underflow, while cend() -1 prevents from returning overflow value
        const auto& it = std::find_if(
            values.cbegin() + 1 + start_index, values.cend() - 1,
            [&input_x](const auto& point)
            {
                return point.first >= input_x;
            }
        );

        x1 = (it - 1)->first;
        y1 = (it - 1)->second;
        x2 = it->first;
        y2 = it->second;

        return std::distance(values.cbegin(), it);
    }

    //! Performs binary search of the provided input_x in the values container.
    //!
    //! @param input_x Input x-axis value to be searched for
    //! @param start_index Index to start the search from
    //! @param x1 X-axis value of the lower section edge
    //! @param y1 Y-axis value of the lower section edge
    //! @param x2 X-axis value of the upper section edge
    //! @param y2 Y-axis value of the upper section edge
    template<typename IndexType, typename StoredType = IndexType>
    inline size_t binary_search(
        std::vector<std::pair<IndexType, StoredType>>& values, IndexType input_x, size_t start_index, IndexType& x1,
        StoredType& y1, IndexType& x2, StoredType& y2
    ) noexcept
    {
        const auto& it = std::upper_bound(
            values.cbegin() + 1 + start_index, values.cend() - 1, input_x,
            [](const auto value, const auto& point)
            {
                return value <= point.first;
            }
        );
        x1 = (it - 1)->first;
        y1 = (it - 1)->second;
        x2 = it->first;
        y2 = it->second;

        return std::distance(values.cbegin(), it);
    }
}