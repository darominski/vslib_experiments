#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

#include "addressRegistry.h"
#include "pid.h"
#include "shared_mem.h"

namespace bkgTask
{
    /**
     * Copies all contents of the currently used buffer to the background buffer to synchronise them.
     */
    void synchroniseReadBuffers()
    {
        auto const& addrRegistrySize = addressRegistry::AddressRegistry::instance().getReadBufferSize();
        auto const& addressRegistry  = addressRegistry::AddressRegistry::instance().getBufferAddrArray();
        for (auto iter = 2 * (bufferSwitch ^ 1); iter < addrRegistrySize; iter += 2)
        {
            memcpy(
                reinterpret_cast<void*>(addressRegistry[iter + bufferSwitch ^ 1].m_addr),
                reinterpret_cast<void*>(addressRegistry[iter + bufferSwitch].m_addr),
                sizeof(reinterpret_cast<void*>(addressRegistry[iter + bufferSwitch].m_addr))
                // TODO: better way to get a size of this memory block, currently fetching void* pointer size
            );
        }
    }

    /**
     * Copies all contents of a write buffer to the background buffer, which is not currently used.
     */
    void copyWriteBuffer()
    {
        auto const& addrRegistrySize    = addressRegistry::AddressRegistry::instance().getWriteBufferSize();
        auto const& writeBufferRegistry = addressRegistry::AddressRegistry::instance().getWriteAddrArray();
        auto const& bkgBufferRegistry   = addressRegistry::AddressRegistry::instance().getBufferAddrArray();

        for (auto iter = 0; iter < addrRegistrySize; iter++)
        {
            auto const& targetBufferAddr
                = bkgBufferRegistry[2 * iter + (bufferSwitch ^ 1)].m_addr;    // only bkg buffer elements are modified
            auto const& writeBufferAddr = writeBufferRegistry[iter].m_addr;   // each write buffer element is visited
            memcpy(
                reinterpret_cast<void*>(targetBufferAddr), reinterpret_cast<void*>(writeBufferAddr),
                sizeof(reinterpret_cast<void*>(writeBufferAddr))
                // TODO: better way to get a size of this memory block, currently fetching void* pointer size
            );
        }
    }
}

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

    // Create and initialize a couple of PIDs
    pid::PID pid1 = pid::PID("pid_1", 1, 1, 1);
    pid::PID pid2 = pid::PID("pid_2", 2, 2, 2);
    pid::PID pid3 = pid::PID("pid_3", 3, 3, 3);

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
        // END TEST CODE
        // TEST CODE, extremely basic execution of commands received from a remote thread
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
        if (counter == 10) break;
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
