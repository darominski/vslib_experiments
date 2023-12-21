#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "bmboot/domain.hpp"
#include "bmboot/domain_helpers.hpp"
#include "bmboot/message_queue.hpp"
#include "json/json.hpp"
#include "sharedMemory.h"
#include "shared_memory.h"

using Json = nlohmann::json;
using namespace vslib;
using namespace fgcd;
using namespace fgc4;

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

int main()
{
    fprintf(stderr, "Start Bmboot\n");

    auto domain = bmboot::throwOnError(bmboot::IDomain::open(bmboot::DomainIndex::cpu1), "IDomain::open");
    bmboot::throwOnError(domain->ensureReadyToLoadPayload(), "ensureReadyToLoadPayload");

    fprintf(stderr, "Map memory\n");
    constexpr int queue_size = fgc4::utils::constants::json_memory_pool_size;

    int          fd = open("/dev/mem", O_RDWR);
    bmboot::Mmap buffer(nullptr, 64 * queue_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, app_data_0_1_ADDRESS);

    fprintf(stderr, "Init message queues\n");
    // one read queue for reading the parameter map and one queue for writing commands

    auto write_commands_queue = bmboot::createMessageQueue<bmboot::MessageQueueWriter<SharedMemoryHeader>>(
        (uint8_t*)buffer.data(), queue_size
    );
    auto read_parameter_map_queue = bmboot::createMessageQueue<bmboot::MessageQueueReader<SharedMemoryHeader>>(
        (uint8_t*)buffer.data() + queue_size, queue_size
    );

    fprintf(stderr, "Run payload\n");
    bmboot::loadPayloadFromFileOrThrow(*domain, "vloop_cpu1.bin");
    usleep(500'000);   // delay for initialization

    std::array<uint8_t, queue_size> parameter_map_buffer;
    std::vector<Json>               commands;
    bool                            commands_set = false;
    size_t                          counter      = 0;
    while (true)
    {
        std::cout << "Linux counter: " << counter++ << "\n";
        // TEST CODE FOR TRANSFERRING COMMANDS
        // there are 3 PID with 9 params + RST with 1 parameter, so 10 in total,
        // modulo prevents setting not used fields
        fprintf(stderr, "Read parameter map\n");
        auto message = read_parameter_map_queue.read(parameter_map_buffer);
        if (message.has_value())
        {
            auto const json_manifest = vslib::readJsonFromMessageQueue(message.value());
            std::cout << json_manifest.dump(1) << "\n";
            auto const settable_parameters = parseManifest(json_manifest);
            commands                       = prepareCommands(settable_parameters);
            commands_set                   = true;
        }
        else
        {
            std::cerr << "No parameter map!\n";
        }

        if (commands_set)
        {
            writeJsonToMessageQueue(commands[counter], write_commands_queue);
        }
        // END OF TEST CODE

        // Add some delay to simulate work
        usleep(1'000'000);   // 1 s
    }

    return 0;
}