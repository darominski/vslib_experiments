//! @file
//! @brief File containing a base class for a thin-layer interface to configure interrupts in VSlib.
//! @author Dominik Arominski

#pragma once

#include <functional>

#include "histogram.h"
#include "pollCpuClock.h"

namespace vslib
{

    class Interrupt
    {
      public:
        //! Constructor for the Interrupt object
        //!
        //! @param handler_function Function to be executed when an interrupt is triggered
        Interrupt(std::function<void(void)> handler_function)
        {
#ifdef PERFORMANCE_TESTS
            m_interrupt_handler = [this, handler_function]()
            {
                const auto start_time = preConditions();
                handler_function();
                const auto total_time = postConditions(start_time);   // and this value needs to be sent somewhere
                if (m_measurement_counter < 1000)
                {
                    m_measurements[m_measurement_counter] = total_time;
                }
                m_measurement_counter++;
            };
            m_measurements = {0};   // sets all elements to 0
#else
            m_interrupt_handler = handler_function;
#endif
        }

        virtual ~Interrupt() = default;

        //! Starts the interrupt
        virtual void start() = 0;

        //! Stops the interrupt
        virtual void stop() = 0;

#ifdef PERFORMANCE_TESTS

        //! Returns the average of interrupt time measurements
        double average() const
        {
            return calculateAverage(m_measurements);
        }

        //! Returns the standard deviation of interrupt time measurements
        double standardDeviation(const double mean) const
        {
            return calculateStandardDeviation(m_measurments, mean);
        }

        //! Returns the histogram with interrupt time measurements
        template<size_t nBins = 11>   // log2(1000) + 1 = 11: Sturges' formula for optimal number of bins
        Histogram<nBins> histogramMeasurements(double min, double max) const
        {
            auto histogram = Histogram<nBins>(min, max);
            fillHistogram(histogram, m_measurements);
            return std::move(histogram);
        }

#endif
      protected:
        std::function<void(void)> m_interrupt_handler;

#ifdef PERFORMANCE_TESTS
        std::array<int64_t, 1000> m_measurements;

        //! Defines the preconditions necessary to estimate the execution time of the interrupt handler
        //!
        //! @return Clock value at the time of the call
        uint64_t preConditions()
        {
            return fgc4::utils::read_CNTPCT();
        }

        //! Defines the postconditions necessary to estimate the execution time of the interrupt handler
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
