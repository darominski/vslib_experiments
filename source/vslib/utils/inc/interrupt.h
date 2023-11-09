//! @file
//! @brief File containing a base class for a thin-layer interface to configure interrupts in VSlib.
//! @author Dominik Arominski

#pragma once

#include <cassert>
#include <functional>

#include "bmboot/payload_runtime.hpp"
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
                m_measurement_counter++;
                const auto start_time = preConditions();
                handler_function();
                const auto total_time = postConditions(start_time);   // and this value needs to be sent somewhere
                if (m_measurement_counter < 1000)
                {
                    m_measurements[m_measurement_counter] = total_time;
                }
            };
            m_measurements = {0};   // sets all elements to 0
#else
            m_interrupt_handler = handler_function;
#endif
            m_current_interrupt_id = m_interrupt_id++;
        }

        virtual ~Interrupt() = default;

        //! Starts the interrupt
        virtual void start() = 0;

        //! Stops the interrupt
        virtual void stop() = 0;

#ifdef PERFORMANCE_TESTS
        //! Allows for benchmarking execution time of the interrupt as an average of the measured times
        double benchmarkInterrupt() const
        {
            auto const denominator
                = static_cast<double>(std::min(static_cast<int32_t>(m_measurements.size()), m_measurement_counter));
            return static_cast<double>(std::accumulate(m_measurements.cbegin(), m_measurements.cend(), 0))
                / denominator;
        }
#endif
      protected:
        std::function<void(void)> m_interrupt_handler;
        int                       m_current_interrupt_id{0};
        inline static int         m_interrupt_id{0};

#ifdef PERFORMANCE_TESTS
        int32_t                   m_measurement_counter{0};
        std::array<int64_t, 1000> m_measurements;

        //! Defines the preconditions necessary to estimate the execution time of the interrupt handler
        //!
        //! @return Clock value at the time of the call
        int64_t preConditions()
        {
            return fgc4::utils::read_CNTPCT();
        }

        //! Defines the postconditions necessary to estimate the execution time of the interrupt handler
        //!
        //! @param starting_point Clock value at the start of the interrupt
        //! @return Difference between the starting and current clock values
        int64_t postConditions(int64_t starting_point)
        {
            // implementation of polling the CPU clock is required
            return fgc4::utils::read_CNTPCT() - starting_point;
        }
#endif
    };

}   // namespace vslib