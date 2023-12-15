//! @file
//! @brief File containing a simple histogram data structure
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cmath>
#include <utility>

#include "statistics.h"

namespace vslib::utils
{

    template<size_t BinNumber>
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

        //! Adds the provided value to the histogram
        //!
        //! @param value New value to be added to the histogram
        void addValue(double value) noexcept
        {
            const int64_t bin_index = std::floor((value - m_min_value) / m_bin_width);
            if (bin_index < 0)   // underflow case
            {
                m_counts[0]++;
            }
            else if (bin_index >= BinNumber)   // overflow case
            {
                m_counts[BinNumber - 1]++;
            }
            else   // regular case
            {
                m_counts[bin_index]++;
            }
        }

        //! Returns the bin value with the maximum value of counts
        //!
        //! @return Bin number where the maximal number of counts is stored
        [[nodiscard]] size_t getBinWithMax() const noexcept
        {
            return std::distance(m_counts.cbegin(), std::max_element(m_counts.cbegin(), m_counts.cend()));
        }

        //! Returns the bin edges values for the provided bin_number. If the bin number is above the number of bins, it
        //! returns the last bin
        //!
        //! @param bin_number Bin number of interest
        [[nodiscard]] std::pair<double, double> getBinEdges(size_t bin_number) const noexcept
        {
            if (bin_number > BinNumber)
            {
                bin_number = BinNumber;
            }
            return std::make_pair(m_edges[bin_number], m_edges[bin_number + 1]);
        }

        //! Returns the data stored in the histogram
        [[nodiscard]] const auto& getData() const noexcept
        {
            return m_counts;
        }

        //! Returns the number of bins of the histogram
        [[nodiscard]] const auto getBinNumber() const noexcept
        {
            return BinNumber;
        }

        //! Returns the bin width of the histogram
        [[nodiscard]] const auto& getBinWidth() const noexcept
        {
            return m_bin_width;
        }

      private:
        std::array<double, BinNumber + 1> m_edges{0};
        std::array<int64_t, BinNumber>    m_counts{0};
        double                            m_min_value;
        double                            m_max_value;
        double                            m_bin_width{0};

        void prepareHistogram()
        {
            m_bin_width = (m_max_value - m_min_value) / BinNumber;
            for (size_t index = 0; index < BinNumber; index++)
            {
                m_edges[index] = m_min_value + static_cast<double>(index) * m_bin_width;
            }
            m_edges[BinNumber] = m_max_value;
        }
    };

}   // namespace vslib::utils
