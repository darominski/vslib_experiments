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
    const char* memoryAddress     = "/shared_mem";
    int         sharedMemoryField = shm_open(memoryAddress, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (sharedMemoryField == -1)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        return 1;
    }

    // Set the size of shared memory region
    size_t size = sizeof(SharedMemory) + (sizeof(parameters::AddressStruct) * parameters::max_registry_size);
    if (ftruncate(sharedMemoryField, size) == -1)
    {
        std::cerr << "Failed to set the size of shared memory" << std::endl;
        close(sharedMemoryField);
        shm_unlink(memoryAddress);
        return 1;
    }

    // Map the shared memory region into the address space
    void* sharedMemory = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, sharedMemoryField, 0);
    if (sharedMemory == MAP_FAILED)
    {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(sharedMemoryField);
        shm_unlink(memoryAddress);
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
    SharedMemory* sharedMemoryRegister = static_cast<SharedMemory*>(sharedMemory);
    sharedMemoryRegister->addressList
        = parameters::AddressRegistry::instance().getWriteAddrArray();   // copies the address array to shared
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
        if (sharedMemoryRegister->transmissionCounter > sharedMemoryRegister->acknowledgeCounter)
        {
            // copy the command into the write buffer
            auto const memoryAddress = reinterpret_cast<void*>(sharedMemoryRegister->commandAddress);
            memcpy(memoryAddress, &sharedMemoryRegister->commandVal, sharedMemoryRegister->commandSize);
            // copy the entire write buffer into the background buffer
            backgroundTask::copyWriteBuffer();
            // switch buffers
            bufferSwitch ^= 1;   // flip the buffer pointer of all variables
            // synchronise the memory between buffers
            backgroundTask::synchroniseReadBuffers();
            // acknowledge transaction
            sharedMemoryRegister->acknowledgeCounter++;
        }
        // END TEST CODE
        // Add some delay to simulate work
        usleep(1000000);
        if (counter == 16) break;
    }

    // Unmap the shared memory region
    if (munmap(sharedMemory, size) == -1)
    {
        std::cerr << "Failed to unmap shared memory" << std::endl;
        return 1;
    }

    // Close and unlink the shared memory object
    close(sharedMemoryField);
    shm_unlink(memoryAddress);

    return 0;
}
