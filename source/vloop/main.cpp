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
#include "logString.h"
#include "nlohmann/json.hpp"
#include "parameterRegistry.h"
#include "pid.h"
#include "rst.h"
#include "sharedMemory.h"
#include "staticJson.h"

// This is one way to stop users from creating objects on the heap and explicit memory allocations
#ifdef __GNUC__
// poisons dynamic memory functions
#pragma GCC poison malloc new
#endif

using namespace vslib;

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
    auto const& json_component_registry = components::ComponentRegistry::instance().createManifest();
    std::cout << json_component_registry.dump() << "\n";
    writeJsonToSharedMemory(json_component_registry, &(SHARED_MEMORY));
    // Create and initialize the shared data structure

    bool received_new_data = false;
    int  counter           = 0;
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

        // END TEST CODE
        // TEST CODE, extremely basic execution of commands received from a remote thread, will be part of background
        // task
        if (SHARED_MEMORY.transmitted_counter > SHARED_MEMORY.acknowledged_counter)
        {
            auto json_object = utils::StaticJsonFactory::getJsonObject();
            json_object      = readJsonFromSharedMemory(&(SHARED_MEMORY));
            // execute the command from the incoming stream, synchronises write and background buffers
            backgroundTask::processJsonCommands(json_object);

            // acknowledge transaction
            SHARED_MEMORY.acknowledged_counter++;
            received_new_data = true;
        }
        else if (received_new_data)
        {
            // if no new data came in the previous iteration, assume it is safe to switch the read buffers now and
            // synchronise them
            buffer_switch ^= 1;   // flip the buffer pointer of all settable parameters
            // synchronise new background to new active buffer
            backgroundTask::synchroniseReadBuffers();
            received_new_data = false;   // buffers updated, no new data available
        }
        // END TEST CODE
        // Add some delay to simulate work
        usleep(1000000);   // 1 s
    }

    return 0;
}
