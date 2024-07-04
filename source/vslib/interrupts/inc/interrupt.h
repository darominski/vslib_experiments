//! @file
//! @brief File containing a base class for a thin-layer interface to configure interrupts in VSlib.
//! @author Dominik Arominski

#pragma once

#include <functional>

#include "histogram.h"
#include "pollCpuClock.h"

namespace vslib
{
#ifdef PERFORMANCE_TESTS
    static constexpr int number_measurements = 1'000;
#endif

    template<class Converter>
    class Interrupt
    {
      public:
        //! Constructor for the Interrupt class.
        //!
        //! @param handler_function Function to be executed when an interrupt is triggered
        Interrupt(std::string_view name, Converter* converter, std::function<void(Converter&)> handler_function)
            : m_name(name)
        {
#ifdef PERFORMANCE_TESTS
            m_measurement_counter = 0;
            m_interrupt_handler   = [this, handler_function, converter]()
            {
                const auto start_time = preConditions();
                handler_function(*converter);
                const auto total_time = postConditions(start_time);
                if (m_measurement_counter < number_measurements)
                {
                    m_measurements[m_measurement_counter % number_measurements] = total_time;
                    m_measurement_counter++;
                }
            };
            m_measurements = {0};   // sets all elements to 0
#else
            m_interrupt_handler = [converter]()
            {
                handler_function(*converter)
            };
#endif
        }

        virtual ~Interrupt() = default;

        //! Starts the interrupt
        virtual void start() = 0;

        //! Stops the interrupt
        virtual void stop() = 0;

#ifdef PERFORMANCE_TESTS

        //! Returns the average of interrupt time measurements.
        double average() const
        {
            return utils::calculateAverage(m_measurements);
        }

        //! Returns the standard deviation of interrupt time measurements.
        //!
        //! @param mean Average of interrupt time measurements
        double standardDeviation(const double mean) const
        {
            return utils::calculateStandardDeviation(m_measurements, mean);
        }

        //! Returns the histogram with interrupt time measurements.
        //!
        //! @param min Minimum range for the histogram
        //! @param max Maximum range for the histogram
        //! @return Histogrammed data
        template<size_t nBins = 32>   // std::ceil(sqrt(1000)) = 32
        utils::Histogram<nBins> histogramMeasurements(double min, double max) const
        {
            auto histogram = utils::Histogram<nBins>(min, max);
            for (auto const& value : m_measurements)
            {
                histogram.addValue(value);
            }
            return histogram;
        }

#endif
      protected:
        std::string const         m_name;                //!< Interrupt name
        std::function<void(void)> m_interrupt_handler;   //!< Function to be called during the interrupt

#ifdef PERFORMANCE_TESTS
        std::array<uint64_t, number_measurements> m_measurements{0};   //!< Container holding interrupt execution time
        int32_t                                   m_measurement_counter;   //!< Counter of interrupt calls

        //! Defines the preconditions necessary to estimate the execution time of the interrupt handler.
        //!
        //! @return Clock value at the time of the call
        uint64_t preConditions()
        {
            return fgc4::utils::read_CNTPCT();
        }

        //! Defines the postconditions necessary to estimate the execution time of the interrupt handler.
        //!
        //! @param starting_point Clock value at the start of the interrupt
        //! @return Difference between the starting and current clock values
        uint64_t postConditions(uint64_t starting_point)
        {
            // implementation of polling the CPU clock is required
            return fgc4::utils::read_CNTPCT() - starting_point;
        }
#endif
    };

}   // namespace vslib
