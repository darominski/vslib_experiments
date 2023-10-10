#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "Util_Mmap.h"
#include "nlohmann/json.hpp"
#include "sharedMemoryVslib.h"

using Json = nlohmann::json;
using namespace vslib;
using namespace fgcd;

auto parseManifest(const Json& manifest)
{
    std::vector<std::pair<std::string, std::string>> settable_parameters;
    for (auto const& element : manifest)
    {
        auto const& parameters        = element["parameters"];   // array of settable parameters;
        auto const& full_component_id = std::string(element["type"]) + "." + std::string(element["name"]);
        for (auto const& parameter : parameters)
        {
            settable_parameters.emplace_back(
                std::make_pair(full_component_id + "." + std::string(parameter["name"]), parameter["type"])
            );
        }
    }
    return settable_parameters;
}

auto prepareCommands(const std::vector<std::pair<std::string, std::string>>& parameters)
{
    std::vector<Json> commands;
    for (const auto& [name, type] : parameters)
    {
        Json command = {{"name", name}};
        if (type == "Float64")
        {
            // half of the values will be invalid
            if (commands.size() % 2 == 0)
            {
                double value = 3.14159 * static_cast<double>(commands.size());
                command.push_back({"value", value});
            }
            else
            {
                std::string value = "invalid";
                command.push_back({"value", value});
            }
        }
        else if (type == "Bool")
        {
            command.push_back({"value", true});
        }
        else if (type == "Enum")
        {
            command.push_back({"value", "updating"});
        }
        else
        {   // Float32Array
            int const             counter = static_cast<int>(commands.size());
            std::array<double, 4> value   = {counter + 0.1, counter + 1.1, counter + 2.2, counter + 3.3};
            command.push_back({"value", value});
        }
        commands.push_back(command);
    }
    commands.push_back({"misformed", "command"});
    commands.push_back({{"name", "ComponentArray.brick_2.PID.brick_2_1.p"}, {"value", 0.2718}});
    return commands;
}

class Fgc4Shmem
{
  public:
    Fgc4Shmem()
        : m_dev_mem("/dev/mem"),
          m_shared_memory(
              m_dev_mem, SHARED_MEMORY_ADDRESS, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, (void*)SHARED_MEMORY_ADDRESS
          )
    {
    }

  private:
    File m_dev_mem;
    Mmap m_shared_memory;
};

int main()
{
    Fgc4Shmem shared_memory;

    auto const json_manifest = readJsonFromSharedMemory(&SHARED_MEMORY);
    std::cout << json_manifest.dump(4) << "\n";
    auto const settable_parameters = parseManifest(json_manifest);
    auto const commands            = prepareCommands(settable_parameters);

    size_t counter = 0;
    while (true)
    {
        if (SHARED_MEMORY.acknowledged_counter < SHARED_MEMORY.transmitted_counter)
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
        writeJsonToSharedMemory(commands[counter], &SHARED_MEMORY);
        SHARED_MEMORY.transmitted_counter++;
        // END OF TEST CODE

        counter++;
        // Add some delay to simulate work
        usleep(1000000);   // 1 s
    }

    return 0;
}