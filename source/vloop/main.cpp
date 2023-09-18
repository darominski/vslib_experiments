#include <algorithm>
#include <array>
#include <bmboot/payload_runtime.hpp>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#include "background.h"
#include "componentRegistry.h"
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

    puts("");

    puts("Hello world from vloop running on cpu1!");

    // ************************************************************
    // Create and initialize a couple of components: 3 PIDs and an RST
    component::PID pid1("pid_1", 1, 1, 1);
    component::PID pid2("pid_2", 2, 2, 2);
    component::PID pid3("pid_3", 3, 3, 3);

    // Freestanding parameters are not permitted, so Parameter<T> param("name", value); declarations are not allowed
    // However, the following is allowed but not a recommended approach:
    // parameters::Parameter<int> pid_1_index(pid1, "index", 1, 0, 1000);

    component::RST rst1("rst_1", {1.1, 2.2, 3.3, 4.4}, false);

    (std::to_string(rst1.r[0]).c_str());   // square bracket access to underlying array

    // No parameter declarations beyond this point!
    // ************************************************************

    puts("Component manifest:");
    auto const& json_component_registry = component::ComponentRegistry::instance().createManifest();
    puts(std::to_string(json_component_registry.dump().size()).c_str());
    bmboot::writeToStdout(
        (json_component_registry.dump()).data(), (json_component_registry.dump().size() * sizeof(char))
    );
    puts("");
    // Create and initialize the shared data structure
    size_t offset
        = 2 * sizeof(size_t);   // Starting offset in the shared memory, size of two counters in the shared memory

    // writeJsonToSharedMemory(json_component_registry, shared_memory, offset, total_memory_size);
    // offset += json_component_registry.dump().size();   // no shared memory cleanup at this time

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
        if (shared_memory_counters->transmitted_counter > shared_memory_counters->acknowledged_counter)
        {
            puts("Received new data!");
            //     auto json_object = utils::StaticJsonFactory::getJsonObject();
            //     json_object      = readJsonFromSharedMemory(shared_memory, offset);
            //     offset           += json_object.dump().size();
            //     // execute the command from the incoming stream, synchronises write and background buffers
            //     background_task::processJsonCommands(json_object);

            // acknowledge transaction
            //     shared_memory_counters->acknowledged_counter++;
            //     received_new_data = true;
        }
        else if (received_new_data)
        {
            //     // if no new data came in the previous iteration, assume it is safe to switch the read buffers now
            //     and
            //     // synchronise them
            //     buffer_switch ^= 1;   // flip the buffer pointer of all settable parameters
            //     // synchronise new background to new active buffer
            //     background_task::synchroniseReadBuffers();
            //     received_new_data = false;   // buffers updated, no new data available
        }

        // END TEST CODE
        // Add some delay to simulate work
        usleep(1000000);   // 1 s
        if (counter == 17) break;
    }

    // // Unmap the shared memory region
    // if (munmap(shared_memory, total_memory_size) == -1)
    // {
    //     utils::LogString error_msg("ERROR: Failed to unmap shared memory.\n");
    //     std::cerr << error_msg;
    //     return 1;
    // }

    // // Close and unlink the shared memory object
    // close(shared_memory_field);
    // shm_unlink(memory_address);

    return 0;
}
