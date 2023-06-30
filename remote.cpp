#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <variant>

#include "addressRegistry.h"
#include "pid.h"
#include "shared_mem.h"

int main()
{
    // Create shared memory region
    const char* memAddr = "/shared_mem";
    int         shmFd   = shm_open(memAddr, O_RDWR, S_IRUSR | S_IWUSR);
    if (shmFd == -1)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    // Calculate the size of shared memory region
    size_t size = sizeof(SharedMem) + (sizeof(addressRegistry::AddressStruct) * addressRegistry::addressRegistrySize);
    // Map the shared memory region into the address space
    void* sharedMem = mmap(NULL, size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, shmFd, 0);
    if (sharedMem == MAP_FAILED)
    {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    // Create and initialize the shared data structure
    SharedMem* sharedMemRegister = static_cast<SharedMem*>(sharedMem);
    auto const addressRegister   = sharedMemRegister->addrRegistry;

    // Access the shared data from a different core
    // In this example, we'll simply increment the value
    int counter = 0;
    while (true)
    {
        // TEST CODE FOR TRANSFERRING COMMANDS
        std::cout << "Thread2 counter: " << counter++ << "\n";
        double const val               = static_cast<double>(counter) * 3.14159;
        // there are 3 PID with 9 params total in the example, modulo prevents setting not used fields
        intptr_t const addr            = addressRegister[counter % 9].m_addr;
        sharedMemRegister->commandAddr = addr;
        sharedMemRegister->commandVal  = val;
        sharedMemRegister->commandSize = sizeof(val);
        sharedMemRegister->transmissionCntr++;
        // END OF TEST CODE

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
