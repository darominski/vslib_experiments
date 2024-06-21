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
#include "rst.h"
#include "state.h"
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
    vslib::PID controller("pid", nullptr);
    // vslib::IIRFilter<81> filter("filter");

    void realTimeTask()
    {
        for (int index = 0; index < 100; index++)
        {
            // volatile double const input = 2.0 * M_PI * (std::rand() / static_cast<double>(RAND_MAX));

            volatile double const input    = index;
            volatile auto         variable = controller.control(input, input + 2);
        }
    }

    enum class ControllerStates
    {
        cycling,
        precharge
    };

    void onCycling()
    {
        std::cout << "cycling!\n";
    }

    using TransRes = ::utils::FsmTransitionResult<ControllerStates>;

    TransRes toPreCharge()
    {
        std::cout << "to pre-charge!\n";
        return {ControllerStates::precharge};
    }

    void setParameters(vslib::PID& controller, TimerInterrupt& timer)
    {
        const double p  = 52.79;
        const double i  = 0.0472;
        const double d  = 0.04406;
        const double ff = 6.1190;
        const double b  = 0.03057;
        const double c  = 0.8983;
        const double N  = 17.79;
        const double ts = 1.0e-3;
        const double f0 = 1e-15;

        controller.actuation_limits.min.setJsonValue(-100);
        controller.actuation_limits.max.setJsonValue(100);
        controller.actuation_limits.dead_zone.setJsonValue(std::array<double, 2>{0, 0});
        controller.actuation_limits.verifyParameters();
        controller.actuation_limits.flipBufferState();

        controller.kp.setJsonValue(p);
        controller.kd.setJsonValue(d);
        controller.ki.setJsonValue(i);
        controller.kff.setJsonValue(ff);
        controller.b.setJsonValue(b);
        controller.c.setJsonValue(c);
        controller.N.setJsonValue(N);
        controller.f0.setJsonValue(f0);
        controller.ts.setJsonValue(ts);

        controller.verifyParameters();
        controller.flipBufferState();

        const int      interrupt_delay = 100;   // us
        nlohmann::json value           = {interrupt_delay};
        timer.delay.setJsonValue(value[0]);
        timer.flipBufferState();
        timer.delay.syncWriteBuffer();
        timer.verifyParameters();
    }

}   // namespace user

// extern user::Converter converter;

int main()
{
    vslib::utils::VSMachine vs_state;   // initial state: initalization

    // VSlib-side initialization:
    bmboot::notifyPayloadStarted();
    puts("Hello world from vloop running on cpu1!");
    Component root("root", "root", nullptr);

    constexpr size_t read_commands_queue_address = app_data_0_1_ADDRESS;
    constexpr size_t write_commands_status_queue_address
        = read_commands_queue_address + fgc4::utils::constants::json_memory_pool_size;
    constexpr size_t write_parameter_map_queue_address = read_commands_queue_address
                                                         + fgc4::utils::constants::json_memory_pool_size
                                                         + fgc4::utils::constants::string_memory_pool_size;
    ParameterSetting parameter_setting_task(
        (uint8_t*)read_commands_queue_address, (uint8_t*)write_commands_status_queue_address, root
    );

    ParameterMap parameter_map(
        (uint8_t*)write_parameter_map_queue_address, fgc4::utils::constants::json_memory_pool_size, root
    );

    // User-side initialization:

    // converter.init();

    // 1 us  -> 1 kHz
    // 50 us -> 20 kHz
    // 20 us -> 50 kHz
    // 10 us -> 100 kHz
    // 1 us  -> 1 MHz
    TimerInterrupt timer("timer", &root, user::realTimeTask);

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

    // No Component declarations beyond this point!
    // ************************************************************

    // transition to unconfigured:
    vs_state.update();

    // part of unconfigured state would be to send the parameter map:
    // parameter_map.uploadParameterMap();

    std::cout << std::boolalpha << "Configured? (expected false) " << vs_state.isConfigured()
              << std::endl;   // should be false

    // User-side configuration:
    user::setParameters(user::controller, timer);

    // transition to configured:
    do
    {
        parameter_setting_task.receiveJsonCommand();
        vs_state.update();
        std::cout << std::boolalpha << "Configured? (expected true) " << vs_state.isConfigured()
                  << std::endl;   // should be true
        usleep(500'000);          // 500 ms
    } while (!vs_state.isConfigured());

    // now, the Parameters are configured, control can be handed over to the user FSM while still
    // running a background task

    if (vs_state.isConfigured())
    {
        timer.start();
    }

    int           counter        = 0;
    int           expected_delay = 210;
    int           time_range_min = expected_delay - 20;   // in clock ticks
    int           time_range_max = expected_delay + 20;   // in clock ticks
    constexpr int n_elements     = 1000;

    while (true)
    {
        if (counter == n_elements + 50)
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
        __asm volatile("wfi");
        counter++;
        // parameter_setting_task.receiveJsonCommand();
        // converter.backgroundTask();
        // usleep(500'000);   // 500 ms
    }

    return 0;
}
