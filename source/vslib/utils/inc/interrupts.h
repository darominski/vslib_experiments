//! @file
//! @brief File containing a thin-layer interface to configure interrupts in VSlib.
//! @author Dominik Arominski

#pragma once

#include <cassert>
#include <functional>

#include "bmboot/payload_runtime.hpp"
#include "counters.h"

namespace vslib
{

    class Interrupt
    {
      public:
        //! Constructor for the Interrupt object
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

        //! Enables the interrupt handling of this interrupt
        void enable() const
        {
            bmboot::disableInterruptHandling(m_current_interrupt_id);
        }

        //! Disables the interrupt handling of this interrupt
        void disable() const
        {
            bmboot::disableInterruptHandling(m_current_interrupt_id);
        }

#ifdef PERFORMANCE_TESTS
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
        int64_t preConditions()
        {
            return fgc4::utils::read_CNTPCT();
        }

        //! Defines the postconditions necessary to estimate the execution time of the interrupt handler
        int64_t postConditions(int64_t starting_point)
        {
            // implementation of polling the CPU clock is required
            return fgc4::utils::read_CNTPCT() - starting_point;
        }
#endif
    };

    class TimerInterrupt : public Interrupt
    {
      public:
        TimerInterrupt(std::function<void(void)> handler_function, int microsecond_delay)
            : Interrupt(std::move(handler_function)),
              m_microsecond_delay{microsecond_delay}
        {
            assert((microsecond_delay > 0) && "Delay for the timing interrupt must be a positive number.");
        }

        void start() override
        {
            bmboot::startPeriodicInterrupt(m_microsecond_delay, m_interrupt_handler);
        }

        void stop() override
        {
            bmboot::stopPeriodicInterrupt();
        }

      private:
        int m_microsecond_delay;
    };

}   // namespace vslib