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
    const char* memoryAddress     = "/shared_mem";
    int         sharedMemoryField = shm_open(memoryAddress, O_RDWR, S_IRUSR | S_IWUSR);
    if (sharedMemoryField == -1)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        close(sharedMemoryField);
        shm_unlink(memoryAddress);
        return 1;
    }

    // Calculate the size of shared memory region
    size_t size = sizeof(SharedMemory) + (sizeof(addressRegistry::AddressStruct) * addressRegistry::max_registry_size);
    // Map the shared memory region into the address space
    void* sharedMemory = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, sharedMemoryField, 0);
    if (sharedMemory == MAP_FAILED)
    {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(sharedMemoryField);
        shm_unlink(memoryAddress);
        return 1;
    }

    // Create and initialize the shared data structure
    SharedMemory* sharedMemoryRegister = static_cast<SharedMemory*>(sharedMemory);
    auto const    addressRegister      = sharedMemoryRegister->addressList;

    // Access the shared data from a different core
    // In this example, we'll simply increment the value
    int counter = 0;
    while (true)
    {
        if (sharedMemoryRegister->acknowledgeCounter < sharedMemoryRegister->transmissionCounter)
        {
            // first process not ready to receive more commands, wait and skip to next iteration
            usleep(500000);   // 0.5 s
            continue;
        }
        // TEST CODE FOR TRANSFERRING COMMANDS
        double newValue        = static_cast<double>(counter) * 3.14159;
        // there are 3 PID with 9 params + RST with 1 parameter, so 10 in total,
        // modulo prevents setting not used fields
        intptr_t const address = addressRegister[counter % 10].m_address;
        std::cout << "Thread2 counter: " << counter++ << "\n";
        sharedMemoryRegister->commandAddress = address;
        sharedMemoryRegister->commandVal     = newValue;
        sharedMemoryRegister->commandSize    = sizeof(newValue);
        if (counter % 10 == 0)
        {
            std::array<double, 4> newR{newValue, newValue + 1, newValue + 2, newValue + 3};
            sharedMemoryRegister->commandVal  = newR;
            sharedMemoryRegister->commandSize = sizeof(newR);
        }
        sharedMemoryRegister->transmissionCounter++;
        // END OF TEST CODE

        // Add some delay to simulate work
        usleep(1);   // 1 s
        // if (counter == 10) break;
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
