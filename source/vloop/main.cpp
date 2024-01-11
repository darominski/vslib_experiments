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

#include "background.h"
#include "boxFilter.h"
#include "boxFirstOrderFilter.h"
#include "componentArray.h"
#include "componentRegistry.h"
#include "compositePID.h"
#include "firFilter.h"
#include "firFirstOrderFilter.h"
#include "iirFilter.h"
#include "interruptRegistry.h"
#include "logString.h"
#include "parameterRegistry.h"
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
    // static int rt_counter = 0;
    // BoxFilter<2>  bfilter("box_filter");
    BoxFirstOrderFilter bfilter("box_filter");
    // FIRFilter<2> ffilter("fir_filter");
    // IIRFilter<3>  ifilter("fir_filter");
    // FIRFirstOrderFilter ffilter("fir_fo_filter");

    void realTimeTask()
    {
        for (int index = 0; index < 50; index++)
        {
            // volatile auto variable = std::rand();
            volatile auto variable = bfilter.filter(std::rand());
        }
    }

    static int peripheral_counter = 0;
    void       peripheralTask()
    {
        printf("%dth event\n", ++peripheral_counter);

        usleep(5);   // 5 us
    }

}   // namespace user


int main()
{
    bmboot::notifyPayloadStarted();
    puts("Hello world from vloop running on cpu1!");

    BackgroundTask background_task;

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

    background_task.uploadParameterMap();

    // TimerInterrupt timer(user::realTimeTask, std::chrono::microseconds(40));
    // timer.start();

    //     InterruptRegistry interrupt_registry;
    //     interrupt_registry.registerInterrupt("physical1", user::peripheralTask, 0, InterruptPriority::medium);
    //     interrupt_registry.startInterrupt("physical1");

    int counter = 0;
    //     int time_range_min = 50;    // in clock ticks, 0 us
    //     int time_range_max = 150;   // in clock ticks, equals 5 us
    usleep(500'000);   // 500 ms

    while (true)
    {
        //         if (counter == 100)
        //         {
        //             timer.stop();
        // #ifdef PERFORMANCE_TESTS
        //             double const mean = timer.average();
        //             std::cout << "Average time per interrupt: " << mean << " +- " << timer.standardDeviation(mean) <<
        //             std::endl; auto const histogram = timer.histogramMeasurements<50>(time_range_min,
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
        puts(std::to_string(counter++).c_str());
        //         // TEST CODE, verbose parameters signalling on thread 1
        // puts("PID1: ");
        std::cout << std::string("Kp: ") + std::to_string(pid1.kp) << "\n";
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

        background_task.receiveJsonCommand();
        usleep(500'000);   // 500 ms
    }

    return 0;
}
