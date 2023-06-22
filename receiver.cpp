#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#include "pid.h"

constexpr size_t addressStructSize = sizeof(AddressStruct);
constexpr size_t addressRegisterSize = 100; // max size of settings

int main() {
    // addressRegister = new std::vector<AddressStruct>(100);
    // Create shared memory region
    const char* memAddr = "/shared_mem";
    int shmFd = shm_open(memAddr, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmFd == -1) {
        std::cerr << "Failed to create shared memory." << std::endl;
        return 1;
    }

    // Set the size of shared memory region
    size_t size = addressStructSize * addressRegisterSize;
    std::cout << size << std::endl;
    if (ftruncate(shmFd, size) == -1) {
        std::cerr << "Failed to set the size of shared memory" << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    // Map the shared memory region into the address space
    void* shared_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (shared_mem == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(shmFd);
        shm_unlink(memAddr);
        return 1;
    }

    std::cout << "Assign shared memory to the addressRegister" << "\n";
    // Create and initialize the shared data structure
    addressRegister = static_cast<std::vector<AddressStruct>*>(shared_mem);
    // addressRegister.resize(addressRegisterSize);
    
    std::cout << "Going to create PIDs" << "\n";

    // Create and initialize a couple of PIDs
    PID::PID pid1 = PID::PID("pid_1", 1,1,1);
    PID::PID pid2 = PID::PID("pid_2", 2,2,2);
    PID::PID pid3 = PID::PID("pid_3", 3,3,3);

    std::cout << "Registered all" << "\n";

    // Access the shared data from a different core
    int counter=0;

    while (true) {
        std::cout << counter++ << "\n";
        std::cout << "PID1: " << pid1.getP() << " " << pid1.getI() << " " << pid1.getD() << "\n";
        std::cout << "PID2: " << pid2.getP() << " " << pid2.getI() << " " << pid2.getD() << "\n";
        std::cout << "PID3: " << pid3.getP() << " " << pid3.getI() << " " << pid3.getD() << "\n";

        // for (auto id=0; id<6; id++) {
            // auto el = addressRegister[0];
            // std::cout << addressRegister[id].m_name << "\n";// << std::get<double*>(el.m_addr) << " " << el.m_type << "\n";
        // }
        // Add some delay to simulate work
        usleep(1000000);
    }

    // Unmap the shared memory region
    if (munmap(shared_mem, size) == -1) {
        std::cerr << "Failed to unmap shared memory" << std::endl;
        return 1;
    }

    // Close and unlink the shared memory object
    close(shmFd);
    shm_unlink(memAddr);

    return 0;
}
