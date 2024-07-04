#pragma once

#include "component.h"
#include "converter.h"
// Fill your includes here
#include "pid.h"
#include "timerInterrupt.h"
// End of your includes

namespace user
{

    class Converter : public vslib::IConverter
    {
      public:
        Converter(Component& root) noexcept
            : vslib::IConverter("Example", &root),
              pid_1("pid_1", this),
              interrupt_1("timer_1", this, 10.0, RTTask1)
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PID                       pid_1;
        vslib::TimerInterrupt<Converter> interrupt_1;

        // ...
        // end of your Components

        // Define your Parameters here

        // end of your Parameters

        void init() override
        {
            interrupt_1.start();
        }

        int                  counter        = 0;
        int                  expected_delay = 210;
        int                  time_range_min = expected_delay - 20;   // in clock ticks
        int                  time_range_max = expected_delay + 20;   // in clock ticks
        constexpr static int n_elements     = 1000;

        void backgroundTask() override
        {
            while (true)
            {
                if (counter == n_elements + 50)
                {
                    interrupt_1.stop();
#ifdef PERFORMANCE_TESTS
                    double const mean = interrupt_1.average();
                    std::cout << "Average time per interrupt: " << mean << " +- " << interrupt_1.standardDeviation(mean)
                              << std::endl;
                    auto const histogram = interrupt_1.histogramMeasurements<100>(time_range_min, time_range_max);
                    for (auto const& value : histogram.getData())
                    {
                        std::cout << value << " ";
                    }
                    std::cout << std::endl;
                    auto const bin_with_max = histogram.getBinWithMax();
                    auto const edges        = histogram.getBinEdges(bin_with_max);
                    std::cout << "bin with max: " << bin_with_max
                              << ", centered at: " << 0.5 * (edges.first + edges.second) << std::endl;
#endif
                    break;
                }
                __asm volatile("wfi");
                counter++;
            }
        }

        static void RTTask1(Converter& converter)
        {
            for (int index = 0; index < 100; index++)
            {
                // volatile double const input = 2.0 * M_PI * (std::rand() / static_cast<double>(RAND_MAX));
                volatile double const input    = 1.0;
                volatile auto         variable = converter.pid_1.control(input, input + 2);
            }
        }
    };


}   // namespace user::converter