#include <algorithm>
#include <array>
#include <bmboot/payload_runtime.hpp>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <unistd.h>

#include "background.h"
#include "componentArray.h"
#include "componentRegistry.h"
#include "compositePID.h"
#include "interrupts.h"
#include "json/json.hpp"
#include "logString.h"
#include "parameterRegistry.h"
#include "pid.h"
#include "rst.h"
#include "staticJson.h"

// This is one way to stop users from creating objects on the heap and explicit memory allocations
#ifdef __GNUC__
// poisons dynamic memory functions
#pragma GCC poison malloc new
#endif

using namespace vslib;
using namespace fgc4;

namespace user
{
    static int counter = 0;
    void       realTimeTask()
    {
        printf("%dth event\n", ++counter);

        usleep(5);   // 5 us
    }

}   // namespace user

int main()
{
    bmboot::notifyPayloadStarted();
    puts("Hello world from vloop running on cpu1!");

    // ************************************************************
    // Create and initialize a couple of components: 3 PIDs and an RST
    components::PID pid1("pid_1", components::independent_component);
    components::PID pid3("pid_3", components::independent_component);
    components::RST rst("rst_1", components::independent_component);

    // No parameter declarations beyond this point!
    // ************************************************************

    puts("Parameter map:");
    backgroundTask::uploadParameterMap();

    TimerInterrupt timer(user::realTimeTask, 100);
    timer.start();

    int counter = 0;
    while (true)
    {
        if (counter == 10)
        {
#ifdef PERFORMANCE_TESTS
            std::cout << "Average time per interrupt: " << timer.benchmarkInterrupt() << std::endl;
#endif
            timer.stop();
            break;
        }
        puts(std::to_string(counter++).c_str());
        // TEST CODE, verbose parameters signalling on thread 1
        // puts("PID1: ");
        // puts(std::to_string(pid1.p).c_str());
        // puts(std::to_string(pid1.i).c_str());
        // puts(std::to_string(pid1.d).c_str());
        // puts("PID3: ");
        // puts(std::to_string(pid3.p).c_str());
        // puts(std::to_string(pid3.i).c_str());
        // puts(std::to_string(pid3.d).c_str());
        // puts("RST: ");
        // for (const auto& val : rst.r)
        // {
        //     std::cout << val << " ";
        // }
        // puts("");

        backgroundTask::receiveJsonCommand();
        usleep(500);   // 50 us
        // usleep(1000000);   // 1 s
    }

    return 0;
}
