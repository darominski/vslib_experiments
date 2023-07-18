#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

#include "addressRegistry.h"
#include "background.h"
#include "pid.h"
#include "rst.h"
#include "sharedMemory.h"

int main()
{
    // Create shared memory region
    const char* memory_address      = "/shared_mem";
    int         shared_memory_field = shm_open(memory_address, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shared_memory_field == -1)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        return 1;
    }

    // Set the size of shared memory region
    size_t total_memory_size
        = sizeof(SharedMemory) + (sizeof(parameters::AddressStruct) * parameters::max_registry_size);
    if (ftruncate(shared_memory_field, total_memory_size) == -1)
    {
        std::cerr << "Failed to set the size of shared memory" << std::endl;
        close(shared_memory_field);
        shm_unlink(memory_address);
        return 1;
    }

    // Map the shared memory region into the address space
    void* shared_memory = mmap(NULL, total_memory_size, PROT_WRITE | PROT_READ, MAP_SHARED, shared_memory_field, 0);
    if (shared_memory == MAP_FAILED)
    {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(shared_memory_field);
        shm_unlink(memory_address);
        return 1;
    }

    // ************************************************************

    // Create and initialize a couple of PIDs
    component::PID pid1("pid_1", 1, 1, 1);
    component::PID pid2("pid_2", 2, 2, 2);
    component::PID pid3("pid_3", 3, 3, 3);

    pid1.p() = pid2.p();   // indirectly tests overloaded operators of Param class

    component::RST rst1("rst_1", {1.1, 2.2, 3.3, 4.4});

    // Create and initialize the shared data structure
    SharedMemory* shared_memory_ptr = static_cast<SharedMemory*>(shared_memory);
    shared_memory_ptr->address_list
        = parameters::AddressRegistry::instance().getWriteAddressArray();   // copies the address array to shared
    int counter = 0;
    while (true)
    {
        std::cout << "Thread 1 counter: " << counter++ << "\n";
        // TEST CODE, verbose parameters signalling on thread 1
        std::cout << "PID1: " << pid1.getP() << " " << pid1.getI() << " " << pid1.getD() << "\n";
        std::cout << "PID2: " << pid2.getP() << " " << pid2.getI() << " " << pid2.getD() << "\n";
        std::cout << "PID3: " << pid3.getP() << " " << pid3.getI() << " " << pid3.getD() << "\n";
        std::cout << "RST1: ";
        std::for_each(
            std::begin(rst1.getR()), std::end(rst1.getR()),
            [](const auto& val)
            {
                std::cout << val << " ";
            }
        );
        std::cout << "\n";
        // END TEST CODE
        // TEST CODE, extremely basic execution of commands received from a remote thread, will be part of background
        // task
        if (shared_memory_ptr->transmission_counter > shared_memory_ptr->acknowledge_counter)
        {
            // copy the command into the write buffer
            auto const memory_address = reinterpret_cast<void*>(shared_memory_ptr->command_address);
            memcpy(memory_address, &shared_memory_ptr->command_value, shared_memory_ptr->command_size);
            // copy the entire write buffer into the background buffer
            backgroundTask::copyWriteBuffer();
            // switch buffers
            buffer_switch ^= 1;   // flip the buffer pointer of all variables
            // synchronise the memory between buffers
            backgroundTask::synchroniseReadBuffers();
            // acknowledge transaction
            shared_memory_ptr->acknowledge_counter++;
        }
        // END TEST CODE
        // Add some delay to simulate work
        usleep(10);   // 10 us
        if (counter == 100000) break;
    }

    // Unmap the shared memory region
    if (munmap(shared_memory, total_memory_size) == -1)
    {
        std::cerr << "Failed to unmap shared memory" << std::endl;
        return 1;
    }

    // Close and unlink the shared memory object
    close(shared_memory_field);
    shm_unlink(memory_address);

    return 0;
}
