#include <algorithm>
#include <array>
#include <bmboot/payload_runtime.hpp>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "boxFilter.h"
#include "clarkeTransform.h"
#include "componentArray.h"
#include "firFilter.h"
#include "iirFilter.h"
#include "interruptRegistry.h"
#include "limitRange.h"
#include "logString.h"
#include "lookupTable.h"
#include "parameterMap.h"
#include "parameterRegistry.h"
#include "parameterSetting.h"
#include "parkTransform.h"
#include "periodicLookupTable.h"
#include "pid.h"
#include "pidRst.h"
#include "rst.h"
#include "timerInterrupt.h"
#include "vslib_shared_memory_memmap.h"

// This is one way to stop users from creating objects on the heap and explicit memory allocations
#ifdef __GNUC__
// poisons dynamic memory functions
#pragma GCC poison malloc new
#endif

using namespace vslib;
using namespace fgc4;

namespace user
{
    vslib::PIDRST controller("pid", nullptr);
    // vslib::IIRFilter<81> filter("filter");

    // std::vector<std::pair<double, double>> function()
    // {
    //     constexpr size_t                       length = 1000;
    //     std::vector<std::pair<double, double>> func(length);
    //     for (size_t index = 0; index < length; index++)
    //     {
    //         const double x = index * 2.0 * M_PI / static_cast<double>(length);
    //         func[index]    = std::make_pair(x, sin(x));
    //     }
    //     return func;
    // }

    // PeriodicLookupTable<double> table("table", nullptr, function(), true);

    void realTimeTask()
    {
        for (int index = 0; index < 100; index++)
        {
            // volatile double const input = 2.0 * M_PI * (std::rand() / static_cast<double>(RAND_MAX));
            // volatile double const input = 2.0 * M_PI * (std::rand() / static_cast<double>(RAND_MAX));

            volatile double const input = index;
            // volatile auto         variable = table.interpolate(input);
            // volatile auto variable = sin(input);
            // volatile auto variable      = table[input];
            // volatile auto variable      = filter.filter(input);
            volatile auto variable      = controller.control(input, input + 2);
            // volatile auto variable = limit.limit(input);
        }
    }

}   // namespace user

// namespace user
// {
//     class SpecificConverter; // forward declaration of Specific converter class
// }

// extern user::Converter converter;

int main()
{
    bmboot::notifyPayloadStarted();
    puts("Hello world from vloop running on cpu1!");

    constexpr size_t read_commands_queue_address = app_data_0_1_ADDRESS;
    constexpr size_t write_commands_status_queue_address
        = read_commands_queue_address + fgc4::utils::constants::json_memory_pool_size;
    constexpr size_t write_parameter_map_queue_address = read_commands_queue_address
                                                         + fgc4::utils::constants::json_memory_pool_size
                                                         + fgc4::utils::constants::string_memory_pool_size;

    // ParameterSetting parameter_setting_task(
    //     (uint8_t*)read_commands_queue_address, (uint8_t*)write_commands_status_queue_address,
    //     user::limit
    // );

    // ParameterMap parameter_map(
    //     (uint8_t*)write_parameter_map_queue_address, fgc4::utils::constants::json_memory_pool_size,
    //     user::limit
    // );

    const double p  = 52.79;
    const double i  = 0.0472;
    const double d  = 0.04406;
    const double ff = 6.1190;
    const double b  = 0.03057;
    const double c  = 0.8983;
    const double N  = 17.79;
    const double ts = 1.0e-3;
    const double f0 = 1e-15;

    user::controller.actuation_limits.min.setJsonValue(-100);
    user::controller.actuation_limits.max.setJsonValue(100);
    user::controller.actuation_limits.verifyParameters();
    user::controller.actuation_limits.flipBufferState();

    user::controller.kp.setJsonValue(p);
    user::controller.kd.setJsonValue(d);
    user::controller.ki.setJsonValue(i);
    user::controller.kff.setJsonValue(ff);
    user::controller.b.setJsonValue(b);
    user::controller.c.setJsonValue(c);
    user::controller.N.setJsonValue(N);
    user::controller.f0.setJsonValue(f0);
    user::controller.ts.setJsonValue(ts);

    user::controller.verifyParameters();
    user::controller.flipBufferState();

    // converter.init();

    // ************************************************************
    // Create and initialize a couple of components: 3 PIDs and an RST

    // PID                 pid1("pid_1", independent_component);
    // PID                 pid2("pid_2", independent_component);
    // RST<5>              rst("rst_1", independent_component);
    // PIDRST              pid_rst("pid_rst_1", independent_component);
    // Limit<double, 2, 4> limit_voltage("limit_v", independent_component);
    // PID pid3("pid_3", independent_component);
    // RST rst("rst_1", independent_component);

    // FIRFilter<10> filter("fir_filter");
    // BoxFilter<5>  bfilter("box_filter");

    // std::srand(10);

    // ComponentArray<ComponentArray<PID, 3>, 3> array("brick_2", nullptr);

    // LookupTable<double> table("table", nullptr, function);

    // No parameter declarations beyond this point!
    // ************************************************************

    // std::cout << converter.serialize();
    // parameter_map.uploadParameterMap();
    // 1 us  -> 1 kHz
    // 50 us -> 20 kHz
    // 20 us -> 50 kHz
    // 10 us -> 100 kHz
    // 1 us  -> 1 MHz
    int            interrupt_delay = 100;   // us
    TimerInterrupt timer("timer", independent_component, user::realTimeTask);
    nlohmann::json value = {interrupt_delay};

    timer.delay.setJsonValue(value[0]);
    timer.flipBufferState();
    timer.delay.syncWriteBuffer();
    timer.verifyParameters();

    timer.start();
    // user::realTimeTask();

    int           counter        = 0;
    int           expected_delay = 210;
    int           time_range_min = expected_delay - 20;   // in clock ticks
    int           time_range_max = expected_delay + 20;   // in clock ticks
    constexpr int n_elements     = 1000;

    while (true)
    {
        if (counter == 1)
        // if (counter == n_elements + 50)
        {
            timer.stop();
#ifdef PERFORMANCE_TESTS
            // std::array<int64_t, n_elements> differences{0};
            // int64_t                         starting_value = timer.m_measurements[0];
            // for (size_t index = 0; index < timer.m_measurements.size() - 1; index++)
            // {
            //     int64_t expected_value = starting_value + expected_delay * index;
            //     differences[index]     = timer.m_measurements[index] - expected_value;
            //     // if (differences[index] < 0)
            //     // {
            //     //     differences[index] = expected_delay;   // loop-around hot-fix
            //     // }
            //     if (abs(differences[index]) > 1)
            //     {
            //         std::cout << index << " " << differences[index] << " " << expected_delay << std::endl;
            //     }
            // }
            // for (int index = 0; index < timer.m_measurements.size(); index++)
            // {
            //     timer.m_measurements[index] = differences[index];
            // }
            // timer.m_measurements[n_elements-1] = timer.m_measurements[n_elements-2];
            double const mean = timer.average();
            std::cout << "Average time per interrupt: " << mean << " +- " << timer.standardDeviation(mean) << std::endl;
            auto const histogram = timer.histogramMeasurements<100>(time_range_min, time_range_max);
            for (auto const& value : histogram.getData())
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;
            auto const bin_with_max = histogram.getBinWithMax();
            auto const edges        = histogram.getBinEdges(bin_with_max);
            std::cout << "bin with max: " << bin_with_max << ", centered at: " << 0.5 * (edges.first + edges.second)
                      << std::endl;
#endif
            break;
        }
        // std::cout << counter << std::endl;
        __asm volatile("wfi");
        counter++;
        // parameter_setting_task.receiveJsonCommand();
        // converter.backgroundTask();
        // usleep(500'000);   // 500 ms
    }

    return 0;
}
