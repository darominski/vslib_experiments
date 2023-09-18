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

    puts("Hello world from vloop running on cpu1!");

    // ************************************************************
    // Create and initialize a couple of components: 3 PIDs and an RST
    components::PID pid1("pid_1", constants::independent_component, 1, 1, 1);
    components::PID pid2("pid_2", constants::independent_component, 2, 2, 2);
    components::PID pid3("pid_3", constants::independent_component, 3, 30, 3);
    std::cout << std::boolalpha << (pid1.p > pid2.p) << "\n";

    components::CompositePID brick1("brick_1", constants::independent_component);

    components::ComponentArray<components::PID, 3> brick2("brick_2", constants::independent_component, 2.2, 1.1, 3.3);

    // Freestanding parameters are not permitted, so Parameter<T> param("name", value); declarations are not allowed
    // However, the following is allowed but not a recommended approach:
    // parameters::Parameter<int> pid_1_index(pid1, "index", 1, 0, 1000);

    components::RST rst1("rst_1", constants::independent_component, {1.1, 2.2, 3.3, 4.4}, false);

    (std::to_string(rst1.r[0]).c_str());   // square bracket access to underlying array

    // No parameter declarations beyond this point!
    // ************************************************************

    puts("Component manifest:");
    auto const& json_component_registry = components::ComponentRegistry::instance().createManifest();
    puts(std::to_string(json_component_registry.dump().size()).c_str());
    bmboot::writeToStdout(
        (json_component_registry.dump()).data(), (json_component_registry.dump().size() * sizeof(char))
    );
    puts("");

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
        puts("PID2: ");
        puts(std::to_string(pid2.p).c_str());
        puts(std::to_string(pid2.i).c_str());
        puts(std::to_string(pid2.d).c_str());
        puts("PID3: ");
        puts(std::to_string(pid3.p).c_str());
        puts(std::to_string(pid3.i).c_str());
        puts(std::to_string(pid3.d).c_str());
        puts("RST1: ");

        std::for_each(
            rst1.r.cbegin(), rst1.r.cend(),
            [](auto const& val)
            {
                puts(std::to_string(val).c_str());
            }
        );
        puts(std::string(rst1.getStatusAsStr()).c_str());
        puts(std::to_string(rst1.flag).c_str());

        // END TEST CODE
        // TEST CODE, extremely basic execution of commands received from a remote thread, will be part of background
        // task
        // if (shared_memory->transmitted_counter > shared_memory->acknowledged_counter)
        // {
        //     puts("Received new data!");
        //     auto json_object = utils::StaticJsonFactory::getJsonObject();
        //     json_object      = shared_memory->json_object;
        //     // execute the command from the incoming stream, synchronises write and background buffers
        //     background_task::processJsonCommands(json_object);

        //     // acknowledge transaction
        //     shared_memory->acknowledged_counter++;
        //     received_new_data = true;
        // }
        // else if (received_new_data)
        // {
        //     // if no new data came in the previous iteration, assume it is safe to switch the read buffers now
        //     // and synchronise them
        //     buffer_switch ^= 1;   // flip the buffer pointer of all settable parameters
        //     // synchronise new background to new active buffer
        //     background_task::synchroniseReadBuffers();
        //     received_new_data = false;   // buffers updated, no new data available
        // }

        // END TEST CODE
        // Add some delay to simulate work
        usleep(1000000);   // 1 s
        if (counter == 17) break;
    }

    return 0;
}
