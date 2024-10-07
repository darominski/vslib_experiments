//! @file
//! @brief Defines class for a box-averaging filter.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "filter.h"
#include "fixedPointType.h"

namespace vslib
{
    template<int64_t FilterOrder, double maximalFilteredValue = 1e5>
    class BoxFilter : public Filter
    {
        constexpr static int64_t buffer_length = FilterOrder + 1;

      public:
        //!< 8 is the number of bits per byte, -1 is for the sign
        constexpr uint64_t static fractional_bits = sizeof(int64_t) * 8 - 1 - std::ceil(log2(maximalFilteredValue));

        //! Constructor of the Box filter Component.
        //!
        //! @param name Name of this Component
        //! @param parent Pointer to the parent of this Component
        BoxFilter(std::string_view name, Component& parent)
            : Filter("BoxFilter", name, parent)
        {
            static_assert(FilterOrder >= 1, "Filter order needs to be a positive number larger than zero.");
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        [[nodiscard]] double filter(const double input) override
        {
            auto const oldest_value = m_buffer[m_head];

            m_buffer[m_head] = input;
            m_cumulative     += m_buffer[m_head] - oldest_value;

            m_head++;
            // this if statement and subsequent shift is about 10% more efficient than a modulo operation
            if (m_head >= buffer_length)
            {
                m_head -= buffer_length;
            }

            return (m_cumulative.toDouble()) / buffer_length;
        }

        // ************************************************************
        // Getters

        //! Returns the maximum value that can be filtered by this filter.
        //!
        //! @return Maximal value that can be filter by this filter
        [[nodiscard]] static auto constexpr getMaxInputValue()
        {
            return FixedPoint<fractional_bits>::maximum_value;
        }

      private:
        std::array<FixedPoint<fractional_bits>, buffer_length> m_buffer{0};   //!< Inputs history buffer
        uint64_t                                               m_head{0};     //!< Points to where the oldest entry is
        FixedPoint<fractional_bits>                            m_cumulative{0};   //!< Cumulative of the inputs
    };


    // ************************************************************
    // Partial template specialization for low-order filters
    //
    // Benchmarking showed 126% gain for the first order, and 50% for the 2nd order.

    template<>
    class BoxFilter<1> : public Filter
    {

      public:
        //! Constructor of the box filter component
        BoxFilter(std::string_view name, Component& parent)
            : Filter("BoxFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(const double input) override
        {
            double const result = input + m_previous_value;

            m_previous_value = input;

            return result / 2.0;
        }

        //! Returns the maximum value that can be filtered by this filter
        //!
        //! @return Maximal value that can be filter by this filter
        [[nodiscard]] static auto const getMaxInputValue()
        {
            return std::numeric_limits<double>::max();
        }

      private:
        double m_previous_value{0};   // input value one iteration earlier
    };

    template<>
    class BoxFilter<2> : public Filter
    {

      public:
        //! Constructor of the box filter component
        BoxFilter(std::string_view name, Component& parent)
            : Filter("BoxFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        double filter(const double input) override
        {
            double const result = input + m_previous_value + m_earlier_value;

            m_earlier_value  = m_previous_value;
            m_previous_value = input;

            return result / 3.0;
        }

        //! Returns the maximum value that can be filtered by this filter
        //!
        //! @return Maximal value that can be filter by this filter
        [[nodiscard]] static auto const getMaxInputValue()
        {
            return std::numeric_limits<double>::max();
        }

      private:
        double m_previous_value{0};   // input value one iteration earlier
        double m_earlier_value{0};    // input value two iterations earlier
    };
}   // namespace vslib
