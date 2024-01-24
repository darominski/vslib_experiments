#include <algorithm>
#include <array>
#include <bmboot/message_queue.hpp>
#include <bmboot/payload_runtime.hpp>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <unistd.h>

#include "boxFilter.h"
#include "componentArray.h"
#include "componentRegistry.h"
#include "compositePID.h"
#include "firFilter.h"
#include "iirFilter.h"
#include "interruptRegistry.h"
#include "logString.h"
#include "parameterMap.h"
#include "parameterRegistry.h"
#include "parameterSetting.h"
#include "pid.h"
#include "rst.h"
#include "timerInterrupt.h"

// This is one way to stop users from creating objects on the heap and explicit memory allocations
#ifdef __GNUC__
// poisons dynamic memory functions
#pragma GCC poison malloc new
#endif

using namespace vslib;
using namespace fgc4;

namespace user
{
    BoxFilter<3> filter("filter");

    void realTimeTask()
    {
        for (int index = 0; index < 50; index++)
        {
            volatile auto variable = filter.filter(std::rand());
        }
    }

    // static int peripheral_counter = 0;
    // void       peripheralTask()
    // {
    //     printf("%dth event\n", ++peripheral_counter);

    //     usleep(5);   // 5 us
    // }

}   // namespace user


int main()
{
    bmboot::notifyPayloadStarted();
    puts("Hello world from vloop running on cpu1!");

    ParameterSetting parameter_setting_task;
    ParameterMap     parameter_map;

    // ************************************************************
    // Create and initialize a couple of components: 3 PIDs and an RST

    PID pid1("pid_1", independent_component);
    // PID pid3("pid_3", independent_component);
    // RST rst("rst_1", independent_component);

    // FIRFilter<10> filter("fir_filter");
    // BoxFilter<5>  bfilter("box_filter");

    // std::srand(10);

    // ComponentArray<ComponentArray<PID, 3>, 3> array("brick_2", nullptr);

    // No parameter declarations beyond this point!
    // ************************************************************

    // auto parameter_map = fgc4::utils::StaticJsonFactory::getJsonObject();
    // parameter_map      = (ComponentRegistry::instance().createParameterMap()).dump();

    // write_queue.write({parameter_map, parameter_map.size()}, {});
    std::cout << "Uploading parameter map\n";
    parameter_map.uploadParameterMap();
    // 1 us  -> 1 kHz
    // 50 us -> 20 kHz
    // 20 us -> 50 kHz
    // 10 us -> 100 kHz
    // 1 us  -> 1 MHz
    int interrupt_delay = 50;   // us
    // TimerInterrupt timer(user::realTimeTask, std::chrono::microseconds(interrupt_delay));
    // timer.start();

    //     InterruptRegistry interrupt_registry;
    //     interrupt_registry.registerInterrupt("physical1", user::peripheralTask, 0, InterruptPriority::medium);
    //     interrupt_registry.startInterrupt("physical1");

    int counter              = 0;
    // int expected_value = 70;
    // int time_range_min = expected_value - 100;   // in clock ticks
    // int time_range_max = expected_value + 100;   // in clock ticks
    int expected_delay       = interrupt_delay / 20e-3;
    int time_range_min       = expected_delay - 10;   // in clock ticks
    int time_range_max       = expected_delay + 10;   // in clock ticks
    // usleep(1'000'000);          // 1 s
    constexpr int n_elements = 1'000'000;

    while (true)
    {
        //         if (counter == n_elements + 50)
        //         {
        //             timer.stop();
        // #ifdef PERFORMANCE_TESTS
        //             // std::array<int64_t, n_elements> differences{0};
        //             // int64_t                         starting_value = timer.m_measurements[0];
        //             // for (size_t index = 0; index < timer.m_measurements.size() - 1; index++)
        //             // {
        //             //     int64_t expected_value = starting_value + expected_delay * index;
        //             //     differences[index]     = timer.m_measurements[index] - expected_value;
        //             //     // if (differences[index] < 0)
        //             //     // {
        //             //     //     differences[index] = expected_delay;   // loop-around hot-fix
        //             //     // }
        //             //     if (abs(differences[index]) > 1)
        //             //     {
        //             //         std::cout << index << " " << differences[index] << " " << expected_delay << std::endl;
        //             //     }
        //             // }
        //             // for (int index = 0; index < timer.m_measurements.size(); index++)
        //             // {
        //             //     timer.m_measurements[index] = differences[index];
        //             // }
        //             // timer.m_measurements[n_elements-1] = timer.m_measurements[n_elements-2];
        //             double const mean = timer.average();
        //             std::cout << "Average time per interrupt: " << mean << " +- " << timer.standardDeviation(mean) <<
        //             std::endl; auto const histogram = timer.histogramMeasurements<100>(time_range_min,
        //             time_range_max); for (auto const& value : histogram.getData())
        //             {
        //                 std::cout << value << " ";
        //             }
        //             std::cout << std::endl;
        //             auto const bin_with_max = histogram.getBinWithMax();
        //             auto const edges        = histogram.getBinEdges(bin_with_max);
        //             std::cout << "bin with max: " << bin_with_max << ", centered at: " << 0.5 * (edges.first +
        //             edges.second)
        //                       << std::endl;
        // #endif
        //             break;
        //         }
        // __asm volatile("wfi");
        counter++;
        // puts(std::to_string(counter++).c_str());
        //         // TEST CODE, verbose parameters signalling on thread 1
        // puts("PID1: ");
        // std::cout << std::string("Kp: ") + std::to_string(pid1.kp) << "\n";
        // puts(std::to_string(pid1.ki).c_str());
        // puts(std::to_string(pid1.kd).c_str());
        //         // puts("PID3: ");
        //         // puts(std::to_string(pid3.p).c_str());
        //         // puts(std::to_string(pid3.i).c_str());
        //         // puts(std::to_string(pid3.d).c_str());
        //         // puts("RST: ");
        //         // for (const auto& val : rst.r)
        //         // {
        //         //     std::cout << val << " ";
        //         // }
        //         // puts("");
        parameter_setting_task.receiveJsonCommand();
        usleep(500'000);   // 500 ms
    }

    return 0;
}
