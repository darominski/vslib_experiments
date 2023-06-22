#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <variant>

// Structure for the shared memory register of addresses
enum TYPE {Int32, Float32};
struct AddressStruct {
    AddressStruct() {};
    AddressStruct(const std::string& name, std::variant<int*, double*> addr, TYPE type) : m_name(name), m_addr(addr), m_type(type) {};
    std::string m_name{""};
    std::variant<int*, double*> m_addr;
    TYPE m_type;
};
static std::vector<AddressStruct> addressRegister(100);
static int registerCounter = 0;

int main() {
    // Create shared memory region
    const char* memAddr = "/shared_mem";
    int shm_fd = shm_open(memAddr, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory" << std::endl;
        return 1;
    }

    // Set the size of shared memory region
    size_t size = sizeof(SharedData<double>);
    if (ftruncate(shm_fd, size) == -1) {
        std::cerr << "Failed to set the size of shared memory" << std::endl;
        return 1;
    }

    // Map the shared memory region into the address space
    void* shared_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        return 1;
    }

    // Create and initialize the shared data structure
    SharedData<double>* shared_data = static_cast<SharedData<double>*>(shared_mem);
    // Access the shared data from a different core
    // In this example, we'll simply increment the value
    int counter=0;
    while (true) {
      std::cout << "Thread2: " << counter++ << "\n";
      // Add some delay to simulate work
      usleep(1000000);
    }

    // Unmap the shared memory region
    if (munmap(shared_mem, size) == -1) {
        std::cerr << "Failed to unmap shared memory" << std::endl;
        return 1;
    }

    // Close and unlink the shared memory object
    close(shm_fd);
    shm_unlink(memAddr);

    return 0;
}
