#include <array>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <tuple>
#include <unistd.h>
#include <variant>
#include <vector>

// global constants defining variable sizes
constexpr short  maxNameLength       = 128;   // max length of component name, in characters
constexpr size_t addressRegisterSize = 100;   // max number of possible settings

// Structure for the shared memory register of addresses
enum TYPE
{
    Int32,
    Float32
};

struct AddressStruct
{
    AddressStruct(){};
    AddressStruct(const std::string& name, std::variant<int*, double*> addr, TYPE type)
        : m_addr(addr),
          m_type(type)
    {
        size_t length = name.size();
        length        = length < name.size() ? length : maxNameLength - 1;
        std::copy(name.begin(), name.begin() + length, m_name.begin());
        m_name[length] = '\0';
    };
    std::array<char, 128>       m_name{};
    std::variant<int*, double*> m_addr;
    TYPE                        m_type;
};

struct SharedMem
{
    std::array<AddressStruct, addressRegisterSize> addrRegister;
    int                                            acknowledgeCntr{0};
    int                                            transmissionCntr{0};
    std::variant<int*, double*>                    commandAddr;
    std::variant<int, double>                      commandVal;
};

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
    size_t size     = sizeof(SharedMem) + (sizeof(AddressStruct) * addressRegisterSize);
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
    auto const addressRegister   = sharedMemRegister->addrRegister;

    // Access the shared data from a different core
    // In this example, we'll simply increment the value
    int counter = 0;
    while (true)
    {
        // TEST CODE FOR TRANSFERRING COMMANDS
        std::cout << "Thread2 counter: " << counter++ << "\n";
        double  val                    = static_cast<double>(counter) * 3.14159;
        double* addr                   = std::get<double*>(addressRegister[counter % 9].m_addr
        );   // there are 3 PID with 9 params total in the example
        sharedMemRegister->commandAddr = addr;
        sharedMemRegister->commandVal  = val;
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
