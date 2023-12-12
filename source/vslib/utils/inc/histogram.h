//! @file
//! @brief File containing a simple histogram data structure
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cmath>

#include "statistics.h"

namespace vslib
{

    template<typename T>
    concept Iterable = requires(T x) {
                           x.begin();
                           x.end();
                       };


    template<size_t nBins>
    class Histogram
    {
      public:
        Histogram() = delete;

        Histogram(const auto& data)
        {
            requires Iterable(data)

                         double const min
                = *std::min_element(data.cbegin(), data.cend());
            double const max = *std::max_element(data.cbegin(), data.cend());
            prepareHistogram(min, max);
            fillHistogram(data);
        }

        void addValue(double value)
        {
            for (int index = 0; index <= nBins; index++)
            {
                if (m_edges[index + 1] >= value)
                {
                    m_counts[index] += value;
                    break;
                }
            }
        }

        void fill(auto& data)
        {
            for (const auto& value : data)
            {
                histogram.addValue(value);
            }
        }

      private:
        std::array<double, nBins + 1> m_edges{0};
        std::array<double, nBins>     m_counts{0};
        double                        m_bin_width{0};

        void prepareHistogram(double min, double max)
        {
            m_bin_width = (max - min) / nBins;
            for (int index = 0; index < nBins; index++)
            {
                m_edges[index] = min + index * bin_width;
            }
            m_edges[nBins] = max * 1.1;   // 10 % extra for all possible overflow values
        }
    };

}   // namespace vslib