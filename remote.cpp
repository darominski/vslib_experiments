#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

#include "sharedMemory.h"

int main()
{
    // Create shared memory region
    const char* memory_address      = "/shared_mem";
    int         shared_memory_field = shm_open(memory_address, O_RDWR, S_IRUSR | S_IWUSR);
    if (shared_memory_field == -1)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        close(shared_memory_field);
        shm_unlink(memory_address);
        return 1;
    }

    // Calculate the size of shared memory region
    size_t total_memory_size
        = sizeof(SharedMemory) + (sizeof(parameters::AddressEntry) * parameters::max_registry_size);
    // Map the shared memory region into the address space
    void* shared_memory = mmap(NULL, total_memory_size, PROT_WRITE | PROT_READ, MAP_SHARED, shared_memory_field, 0);
    if (shared_memory == MAP_FAILED)
    {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(shared_memory_field);
        shm_unlink(memory_address);
        return 1;
    }

    // Create and initialize the shared data structure
    SharedMemory* shared_memory_ptr = static_cast<SharedMemory*>(shared_memory);
    auto const    address_registry  = shared_memory_ptr->address_list;

    // Access the shared data from a different core
    // In this example, we'll simply increment the value
    int counter = 0;
    while (true)
    {
        if (shared_memory_ptr->acknowledge_counter < shared_memory_ptr->transmission_counter)
        {
            // first process not ready to receive more commands, wait and skip to next iteration
            usleep(500);   // 0.5 ms
            continue;
        }
        // TEST CODE FOR TRANSFERRING COMMANDS
        double new_value       = static_cast<double>(counter) * 3.14159;
        // there are 3 PID with 9 params + RST with 1 parameter, so 10 in total,
        // modulo prevents setting not used fields
        intptr_t const address = address_registry[counter % 10].m_address;
        std::cout << "Thread2 counter: " << counter++ << "\n";
        shared_memory_ptr->command_address = address;
        shared_memory_ptr->command_value   = new_value;
        shared_memory_ptr->command_size    = sizeof(new_value);
        if (counter % 10 == 0)
        {
            std::array<double, 4> new_r{new_value, new_value + 1, new_value + 2, new_value + 3};
            shared_memory_ptr->command_value = new_r;
            shared_memory_ptr->command_size  = sizeof(new_r);
        }
        shared_memory_ptr->transmission_counter++;
        // END OF TEST CODE

        // Add some delay to simulate work
        usleep(10);   // 100 us
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
