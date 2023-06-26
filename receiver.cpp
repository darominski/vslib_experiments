#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <tuple>
#include <unistd.h>

#include "pid.h"

struct SharedMem
{
    std::array<AddressStruct, addressRegisterSize> addrRegister;
    int                                            acknowledgeCntr{0};
    int                                            transmissionCntr{0};
    std::variant<int*, double*>                    commandAddr;
    std::variant<int, double>                      commandVal;
    size_t                                         commandSize;
};

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
    size_t size = sizeof(SharedMem) + (sizeof(AddressStruct) * addressRegisterSize);
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
    PID::PID pid1 = PID::PID("pid_1", 1, 1, 1);
    PID::PID pid2 = PID::PID("pid_2", 2, 2, 2);
    PID::PID pid3 = PID::PID("pid_3", 3, 3, 3);

    // Create and initialize the shared data structure
    SharedMem* sharedMemRegister    = static_cast<SharedMem*>(sharedMem);
    sharedMemRegister->addrRegister = std::move(addressRegister);

    int counter = 0;
    while (true)
    {
        std::cout << "Thread 1 counter: " << counter++ << "\n";
        // TEST CODE, verbose parameters signalling on thread 1
        std::cout << "PID1: " << pid1.getP() << " " << pid1.getI() << " " << pid1.getD() << "\n";
        std::cout << "PID2: " << pid2.getP() << " " << pid2.getI() << " " << pid2.getD() << "\n";
        std::cout << "PID3: " << pid3.getP() << " " << pid3.getI() << " " << pid3.getD() << "\n";
        // END TEST CODE
        // TEST CODE, extremely basic verbose execution of commands received from a remote thread
        if (sharedMemRegister->transmissionCntr > sharedMemRegister->acknowledgeCntr)
        {
            memcpy(
                std::get<double*>(sharedMemRegister->commandAddr), &std::get<double>(sharedMemRegister->commandVal),
                sharedMemRegister->commandSize
            );
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
