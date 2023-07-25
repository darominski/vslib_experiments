#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

#include "nlohmann/json.hpp"
#include "sharedMemory.h"

using json = nlohmann::json;

auto const parseManifest(const json& manifest)
{
    std::vector<std::pair<std::string, std::string>> settable_parameters;
    for (auto const& element : manifest)
    {
        settable_parameters.push_back(std::make_pair(element["name"], element["type"]));
    }
    return settable_parameters;
}

auto const prepareCommands(std::vector<std::pair<std::string, std::string>> parameters)
{
    std::vector<json> commands;
    for (const auto& [name, type] : parameters)
    {
        json command = {{"name", name}, {"type", type}};
        if (type == "Float32")
        {
            double value = 3.14159 * static_cast<double>(commands.size());
            command.push_back({"value", value});
        }
        else
        {   // Float32Array
            int const             counter = static_cast<int>(commands.size());
            std::array<double, 4> value   = {counter + 0.1, counter + 1.1, counter + 2.2, counter + 3.3};
            command.push_back({"value", value});
        }
        commands.push_back(command);
    }
    return commands;
}

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
    constexpr size_t total_memory_size = 1 * 1024 * 1024;   // 1 MB
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
    SharedMemory* shared_memory_counters = static_cast<SharedMemory*>(shared_memory);

    // Access the shared data from a different core
    // In this example, we'll simply increment the value
    size_t counter = 0;
    size_t offset  = 2 * sizeof(size_t);   // two size_t communication counters

    auto const json_manifest       = readJsonFromSharedMemory(shared_memory, offset);
    offset                         += json_manifest.dump().size();   // no shared memory cleanup at this time
    // offset += sizeof(json_manifest); // no shared memory cleanup at this time
    auto const settable_parameters = parseManifest(json_manifest);
    auto const commands            = prepareCommands(settable_parameters);

    while (true)
    {
        if (shared_memory_counters->acknowledged_counter < shared_memory_counters->transmitted_counter)
        {
            // first process not ready to receive more commands, wait and skip to next iteration
            usleep(500000);   // 0.5 s
            continue;
        }
        std::cout << "Thread2 counter: " << counter << "\n";
        // TEST CODE FOR TRANSFERRING COMMANDS
        // there are 3 PID with 9 params + RST with 1 parameter, so 10 in total,
        // modulo prevents setting not used fields
        if (counter >= commands.size())
        {
            break;
        }
        writeJsonToSharedMemory(commands[counter], shared_memory, offset, total_memory_size);
        offset += (commands[counter].dump().size());   // no shared memory cleanup at this time
        shared_memory_counters->transmitted_counter++;
        // END OF TEST CODE

        counter++;
        // Add some delay to simulate work
        usleep(1000000);   // 1 s
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
