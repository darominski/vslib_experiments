#include <algorithm>
#include <array>
#include <bmboot/payload_runtime.hpp>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#include "background.h"
#include "componentArray.h"
#include "componentRegistry.h"
#include "compositePID.h"
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

int main()
{
    bmboot::notifyPayloadStarted();

    try
    {
        throw std::runtime_error("TEST");
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }

    puts("Hello world from vloop running on cpu1!");

    // ************************************************************
    // Create and initialize a couple of components: 3 PIDs and an RST
    components::PID pid1("pid_1", components::independent_component, 1, 1, 1);
    components::PID pid3("pid_3", components::independent_component, 3, 30, 3);

    // No parameter declarations beyond this point!
    // ************************************************************

    puts("Component manifest:");
    backgroundTask::uploadManifest();

    int counter = 0;
    while (true)
    {
        utils::LogString counter_message = "Thread 1 counter: ";
        puts(counter_message.c_str());
        puts(std::to_string(counter++).c_str());
        // TEST CODE, verbose parameters signalling on thread 1
        puts("PID1: ");
        puts(std::to_string(pid1.p).c_str());
        puts(std::to_string(pid1.i).c_str());
        puts(std::to_string(pid1.d).c_str());
        puts("PID3: ");
        puts(std::to_string(pid3.p).c_str());
        puts(std::to_string(pid3.i).c_str());
        puts(std::to_string(pid3.d).c_str());

        backgroundTask::receiveJsonCommand();
        usleep(1000000);   // 1 s
    }

    return 0;
}
