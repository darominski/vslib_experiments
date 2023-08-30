#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
// #include <sys/mman.h>
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
    // // Create shared memory region
    // const char* memory_address      = "/shared_mem";
    // int         shared_memory_field = shm_open(memory_address, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    // if (shared_memory_field == -1)
    // {
    //     utils::LogString error_msg("ERROR: Failed to create shared memory.\n");
    //     std::cerr << error_msg;
    //     return 1;
    // }

    // // Set the size of shared memory region
    // constexpr auto total_memory_size = static_cast<size_t>(1 * 1024 * 1024);   // 1 MB
    // if (ftruncate(shared_memory_field, total_memory_size) == -1)
    // {
    //     utils::LogString error_msg("ERROR: Failed to set the size of shared memory.\n");
    //     std::cerr << error_msg;
    //     close(shared_memory_field);
    //     shm_unlink(memory_address);
    //     return 1;
    // }

    // // Map the shared memory region into the address space
    // void* shared_memory = mmap(nullptr, total_memory_size, PROT_WRITE | PROT_READ, MAP_SHARED, shared_memory_field,
    // 0); if (shared_memory == MAP_FAILED)
    // {
    //     utils::LogString error_msg("ERROR: Failed to map shared memory.\n");
    //     std::cerr << error_msg;
    //     close(shared_memory_field);
    //     shm_unlink(memory_address);
    //     return 1;
    // }

    // auto* shared_memory_counters = static_cast<SharedMemory*>(shared_memory);

    // ************************************************************
    // Create and initialize a couple of components: 3 PIDs and an RST
    component::PID pid1("pid_1", 1, 1, 1);
    component::PID pid2("pid_2", 2, 2, 2);
    component::PID pid3("pid_3", 3, 3, 3);

    std::cout << std::boolalpha << (pid1.p > pid2.p) << "\n";

    // Freestanding parameters are not permitted, so Parameter<T> param("name", value); declarations are not allowed
    // However, the following is allowed but not a recommended approach:
    // parameters::Parameter<int> pid_1_index(pid1, "index", 1, 0, 1000);

    component::RST rst1("rst_1", {1.1, 2.2, 3.3, 4.4}, false);

    std::cout << rst1.r[0] << "\n";   // square bracket access to underlying array

    // No parameter declarations beyond this point!
    // ************************************************************

    std::cout << "Component manifest:\n";
    auto const& json_component_registry = component::ComponentRegistry::instance().createManifest();
    std::cout << json_component_registry.dump(4) << "\n";
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
        std::cout << counter_message << counter++ << "\n";
        // TEST CODE, verbose parameters signalling on thread 1
        std::cout << "PID1: " << pid1.p << " " << pid1.i << " " << pid1.d << "\n";
        std::cout << "PID2: " << pid2.p << " " << pid2.i << " " << pid2.d << "\n";
        std::cout << "PID3: " << pid3.p << " " << pid3.i << " " << pid3.d << "\n";
        std::cout << "RST1: ";

        std::for_each(
            rst1.r.cbegin(), rst1.r.cend(),
            [](auto const& val)
            {
                std::cout << val << " ";
            }
        );
        std::cout << rst1.getStatusAsStr() << " " << std::boolalpha << rst1.flag << "\n";
        // END TEST CODE
        // TEST CODE, extremely basic execution of commands received from a remote thread, will be part of background
        // task
        // if (shared_memory_counters->transmitted_counter > shared_memory_counters->acknowledged_counter)
        // {
        //     auto json_object = utils::StaticJsonFactory::getJsonObject();
        //     json_object      = readJsonFromSharedMemory(shared_memory, offset);
        //     offset           += json_object.dump().size();
        //     // execute the command from the incoming stream, synchronises write and background buffers
        //     background_task::processJsonCommands(json_object);

        //     // acknowledge transaction
        //     shared_memory_counters->acknowledged_counter++;
        //     received_new_data = true;
        // }
        // else if (received_new_data)
        // {
        //     // if no new data came in the previous iteration, assume it is safe to switch the read buffers now and
        //     // synchronise them
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
