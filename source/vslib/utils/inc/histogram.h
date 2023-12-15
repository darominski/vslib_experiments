//! @file
//! @brief File containing a simple histogram data structure
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cmath>

#include "statistics.h"

namespace vslib
{

    template<size_t nBins>
    class Histogram
    {
      public:
        Histogram() = delete;

        //! Histogram constructor where the minimum and maximum value to be stored is provided.
        //!
        //! @param min Defines the first bin lower edge
        //! @param max Defines the final bin upper edge
        Histogram(double min, double max) noexcept
            : m_min_value(min),
              m_max_value(max)
        {
            prepareHistogram();
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
        std::array<int64_t, nBins>    m_counts{0};
        double                        m_min_value;
        double                        m_max_value;
        double                        m_bin_width{0};

        void prepareHistogram()
        {
            m_bin_width = (m_max_value - m_min_value) / nBins;
            for (size_t index = 0; index < nBins; index++)
            {
                m_edges[index] = m_min_value + static_cast<double>(index) * m_bin_width;
            }
            m_edges[nBins] = m_max_value;
        }
    };

}   // namespace vslib