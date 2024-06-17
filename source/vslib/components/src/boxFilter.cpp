#include "boxFilter.h"

namespace vslib
{
    // Specializations for low-order filters
    //
    // Benchmarking showed 126% gain for the first order, and 50% for the 2nd order.

    template<>
    class BoxFilter<1> : public Filter
    {

      public:
        //! Constructor of the BoxFilter Component.
        //!
        //! @param name Name of this Filter Component
        //! @param parent Parent of this Filter Component
        BoxFilter(std::string_view name, Component* parent)
            : Filter("BoxFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        [[nodiscard]] double filter(double input) override
        {
            double const result = input + m_previous_value;

            m_previous_value = input;

            return result / 2.0;
        }

        //! Returns the maximum value that can be filtered.
        //!
        //! @return Maximal value that can be filtered
        [[nodiscard]] static auto constexpr getMaxInputValue()
        {
            return std::numeric_limits<double>::max();
        }

      private:
        double m_previous_value{0};   //!< input value one iteration earlier
    };

    template<>
    class BoxFilter<2> : public Filter
    {

      public:
        //! Constructor of the box filter component
        BoxFilter(std::string_view name, Component* parent)
            : Filter("BoxFilter", name, parent)
        {
        }

        //! Filters the provided input by calculating the moving average of the buffer of previously
        //! provided inputs.
        //!
        //! @param input Input value to be filtered
        //! @return Filtered value
        [[nodiscard]] double filter(double input) override
        {
            double const result = input + m_previous_value + m_earlier_value;

            m_earlier_value  = m_previous_value;
            m_previous_value = input;

            return result / 3.0;
        }

        //! Returns the maximum value that can be filtered.
        //!
        //! @return Maximal value that can be filtered.
        [[nodiscard]] static auto constexpr getMaxInputValue()
        {
            return std::numeric_limits<double>::max();
        }

      private:
        double m_previous_value{0};   //!< input value one iteration earlier
        double m_earlier_value{0};    //!< input value two iterations earlier
    };
}   // namespace vslib