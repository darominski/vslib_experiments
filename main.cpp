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
#include "shared_mem.h"

int main()
{
    // Create shared memory region
    const char* memAddr = "/shared_mem";
    int         shmFd   = shm_open(memAddr, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmFd == -1)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        return 1;
    }

    // Set the size of shared memory region
    size_t size = sizeof(SharedMem) + (sizeof(addressRegistry::AddressStruct) * addressRegistry::max_registry_size);
    if (ftruncate(shmFd, size) == -1)
    {
        std::cerr << "Failed to set the size of shared memory" << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    // Map the shared memory region into the address space
    void* sharedMem = mmap(NULL, size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, shmFd, 0);
    if (sharedMem == MAP_FAILED)
    {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    // ************************************************************

    // Create and initialize a couple of PIDs
    pid::PID pid1("pid_1", 1, 1, 1);
    pid::PID pid2("pid_2", 2, 2, 2);
    pid::PID pid3("pid_3", 3, 3, 3);

    rst::RST rst1("rst_1", {1.1, 2.2, 3.3, 4.4});

    // Create and initialize the shared data structure
    SharedMem* sharedMemRegister = static_cast<SharedMem*>(sharedMem);
    sharedMemRegister->addrRegistry
        = addressRegistry::AddressRegistry::instance().getWriteAddrArray();   // copies the address array to shared
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
            rst1.getR().cbegin(), rst1.getR().cend(),
            [](const auto& val)
            {
                std::cout << val << " ";
            }
        );
        std::cout << "\n";
        // END TEST CODE
        // TEST CODE, extremely basic execution of commands received from a remote thread, will be part of background
        // task
        if (sharedMemRegister->transmissionCntr > sharedMemRegister->acknowledgeCntr)
        {
            // copy the command into the write buffer
            auto const memAddr = reinterpret_cast<void*>(sharedMemRegister->commandAddr);
            memcpy(memAddr, &sharedMemRegister->commandVal, sharedMemRegister->commandSize);
            // copy the entire write buffer into the background buffer
            bkgTask::copyWriteBuffer();
            // switch buffers
            bufferSwitch ^= 1;   // flip the buffer pointer of all variables
            // synchronise the memory between buffers
            bkgTask::synchroniseReadBuffers();
            // acknowledge transaction
            sharedMemRegister->acknowledgeCntr++;
        }
        // END TEST CODE
        // Add some delay to simulate work
        usleep(1000000);
        if (counter == 16) break;
    }

    // Unmap the shared memory region
    if (munmap(sharedMem, size) == -1)
    {
        std::cerr << "Failed to unmap shared memory" << std::endl;
        return 1;
    }

    // Close and unlink the shared memory object
    close(shmFd);
    shm_unlink(memAddr);

    return 0;
}
