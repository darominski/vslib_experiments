#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include "pid.h"

struct SharedMem {
    std::array<AddressStruct, addressRegisterSize> addrRegisterPtr;
};

int main() {
    // Create shared memory region
    const char* memAddr = "/shared_mem";
    int shmFd = shm_open(memAddr, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmFd == -1) {
        std::cerr << "Failed to create shared memory." << std::endl;
        return 1;
    }

    // Set the size of shared memory region
    size_t size = sizeof(SharedMem) + (sizeof(AddressStruct) * addressRegisterSize);
    std::cout << size << "\n";
    if (ftruncate(shmFd, size) == -1) {
        std::cerr << "Failed to set the size of shared memory" << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    // Map the shared memory region into the address space
    void* sharedMem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (sharedMem == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    std::cout << "Going to create PIDs" << "\n";
    // Create and initialize a couple of PIDs
    PID::PID pid1 = PID::PID("pid_1", 1,1,1);
    PID::PID pid2 = PID::PID("pid_2", 2,2,2);
    PID::PID pid3 = PID::PID("pid_3", 3,3,3); 
    std::cout << "Registered all" << "\n";

    // Create and initialize the shared data structure
    SharedMem* sharedMemRegister = static_cast<SharedMem*>(sharedMem);
    std::cout << "Assign shared memory to the addressRegister" << "\n";
    // sharedMemRegister->addrRegisterPtr.resize(addressRegister.size());
    std::cout << sharedMemRegister->addrRegisterPtr.size() << std::endl;
    sharedMemRegister->addrRegisterPtr = std::move(addressRegister);

   for (auto id=0; id<6; id++) {
        std::cout << "Shared memory before loop\n";
        std::string name = std::string(sharedMemRegister->addrRegisterPtr[id].m_name.data()); 
        std::cout << name  << " " << std::get<double*>(sharedMemRegister->addrRegisterPtr[id].m_addr) << " " << sharedMemRegister->addrRegisterPtr[id].m_type << "\n";
    }

    int counter=0;
    while (true) {
        std::cout << "Thread 1 counter: " << counter++ << "\n";
        std::cout << "PID1: " << pid1.getP() << " " << pid1.getI() << " " << pid1.getD() << "\n";
        std::cout << "PID2: " << pid2.getP() << " " << pid2.getI() << " " << pid2.getD() << "\n";
        std::cout << "PID3: " << pid3.getP() << " " << pid3.getI() << " " << pid3.getD() << "\n";
        // Add some delay to simulate work
        usleep(2000000);
        if (counter == 10)
            break;
    }

    // Unmap the shared memory region
    if (munmap(sharedMem, size) == -1) {
        std::cerr << "Failed to unmap shared memory" << std::endl;
        return 1;
    }

    // Close and unlink the shared memory object
    close(shmFd);
    shm_unlink(memAddr);

    return 0;
}
