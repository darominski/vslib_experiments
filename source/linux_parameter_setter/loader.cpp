#include <array>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>

#include "bmboot/domain.hpp"
#include "bmboot/domain_helpers.hpp"
#include "csv.hpp"
#include "json/json.hpp"
#include "messageQueue.hpp"
#include "shared_memory.h"
#include "vslibMessageQueue.hpp"

using Json = nlohmann::json;
using namespace vslib;
using namespace fgcd;
using namespace fgc4;
using namespace csv;

void parseComponent(
    const Json& component, const std::string& base_name,
    std::vector<std::pair<std::string, std::string>>& settable_parameters
)
{
    if (!component.contains("parameters"))
    {
        return;
    }
    const auto& parameters          = component["parameters"];   // array of settable parameters;
    const auto  full_component_name = base_name + "." + std::string(component["name"]);
    for (const auto& parameter : parameters)
    {
        settable_parameters.emplace_back(
            std::make_pair(full_component_name + "." + std::string(parameter["name"]), parameter["type"])
        );
    }

    // recursively traverse hierarchy:
    if (component.contains("components"))
    {
        for (const auto& child : component["components"])
        {
            parseComponent(child, full_component_name, settable_parameters);
        }
    }
}

auto parseManifest(const Json& manifest)
{
    std::vector<std::pair<std::string, std::string>> settable_parameters;
    for (auto const& element : manifest)
    {
        if (!element.contains("components"))
        {
            continue;
        }
        for (const auto& component : element["components"])
        {
            parseComponent(component, element["name"], settable_parameters);
        }
    }
    return settable_parameters;
}

auto prepareCommands(const std::vector<std::pair<std::string, std::string>>& parameters)
{
    std::vector<Json>           commands;
    std::array<int, 2>          version{0, 1};
    std::map<std::string, Json> values_to_set;

    constexpr double control_period    = 100e-6;   // s
    constexpr double current_frequency = 50;       // Hz

    values_to_set["pll.f_rated"]      = current_frequency;
    values_to_set["pll.angle_offset"] = 0.0;

    values_to_set["pi.kp"]                      = 400.0;
    values_to_set["pi.ki"]                      = 35000.0;
    values_to_set["pi.kd"]                      = 0.0;
    values_to_set["pi.kff"]                     = 0.0;
    values_to_set["pi.proportional_scaling"]    = 1.0;
    values_to_set["pi.derivative_scaling"]      = 1.0;
    values_to_set["pi.derivative_filter_order"] = 1.0;
    values_to_set["pi.control_period"]          = control_period;
    values_to_set["pi.pre_warping_frequency"]   = 1e-12;

    values_to_set["actuation_limits.lower_threshold"] = -1e12;
    values_to_set["actuation_limits.upper_threshold"] = 1e12;
    values_to_set["actuation_limits.dead_zone"]       = std::array<double, 2>{0, 0};

    for (const auto& [name, _] : parameters)
    {
        const std::string param_name = name.substr(name.find_last_of(".") + 1, name.size());
        std::string       comp_name  = (name.substr(0, name.find_last_of(".")));
        comp_name                    = comp_name.substr(comp_name.find_last_of(".") + 1, comp_name.size());

        Json command = {{"name", name}, {"version", version}, {"value", values_to_set[comp_name + "." + param_name]}};
        commands.emplace_back(command);
    }
    return commands;
}

struct DataFrame
{
    uint64_t             clk_cycles;
    std::array<float, 9> data;
};

int main(int argc, char* argv[])
{
    fprintf(stderr, "Start Bmboot\n");

    auto domain = bmboot::throwOnError(bmboot::IDomain::open(bmboot::DomainIndex::cpu3), "IDomain::open");
    bmboot::throwOnError(domain->ensureReadyToLoadPayload(), "ensureReadyToLoadPayload");

    fprintf(stderr, "Map memory\n");
    constexpr int json_queue_size   = fgc4::utils::constants::json_memory_pool_size;
    constexpr int string_queue_size = fgc4::utils::constants::string_memory_pool_size;

    int          fd = open("/dev/mem", O_RDWR);
    bmboot::Mmap buffer(nullptr, 64 * json_queue_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, app_data_2_3_ADDRESS);

    fprintf(stderr, "Init message queues\n");
    // one read queue for reading the parameter map and one queue for writing commands

    constexpr size_t read_commands_queue_address = app_data_2_3_ADDRESS;
    constexpr size_t write_commands_status_queue_address
        = read_commands_queue_address + fgc4::utils::constants::json_memory_pool_size;
    constexpr size_t write_parameter_map_queue_address = read_commands_queue_address
                                                         + fgc4::utils::constants::json_memory_pool_size
                                                         + fgc4::utils::constants::string_memory_pool_size;
    constexpr size_t read_data_queue_address = app_data_2_3_ADDRESS + 3 * fgc4::utils::constants::json_memory_pool_size;

    auto write_commands_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueWriter<void>>(
        (uint8_t*)buffer.data(), json_queue_size
    );

    auto read_command_status_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)buffer.data() + json_queue_size, string_queue_size
    );

    auto read_parameter_map_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)buffer.data() + json_queue_size + string_queue_size, json_queue_size
    );

    auto read_data_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<DataFrame>>(
        (uint8_t*)buffer.data() + json_queue_size * 3, json_queue_size
    );

    // Define the file to output the ADC values coming from the bare metal
    std::filesystem::path adc_output_path = "./adc_output.csv";
    std::ofstream         adc_output_file;
    adc_output_file.open(adc_output_path.c_str());

    fprintf(stderr, "Run payload\n");
    bmboot::loadPayloadFromFileOrThrow(*domain, argv[1]);
    usleep(500'000);   // delay for initialization

    std::array<uint8_t, json_queue_size>   parameter_map_buffer;
    std::array<uint8_t, string_queue_size> command_status_buffer{0};
    std::array<uint8_t, json_queue_size>   data_buffer;

    std::vector<Json> commands;
    bool              commands_set  = false;
    size_t            counter       = 0;
    size_t            commands_sent = 0;
    while (true)
    {
        // std::cout << "Linux counter: " << counter++ << "\n";
        // TEST CODE FOR TRANSFERRING COMMANDS
        // there are 3 PID with 9 params + RST with 1 parameter, so 10 in total,
        // modulo prevents setting not used fields
        auto parameter_map_message = read_parameter_map_queue.read(parameter_map_buffer);
        if (parameter_map_message.has_value())
        {
            auto const json_manifest       = vslib::utils::readJsonFromMessageQueue(parameter_map_message.value());
            // std::cout << json_manifest.dump(1) << "\n";
            auto const settable_parameters = parseManifest(json_manifest);
            commands                       = prepareCommands(settable_parameters);
            commands_set                   = true;
        }

        if (commands_set && (commands_sent <= commands.size()))
        {
            // check status of previous command
            auto status_message = read_command_status_queue.read(command_status_buffer);
            if (status_message.has_value())
            {

                std::cout << "Status: " << std::string(status_message->begin(), status_message->end()) << std::endl;
            }
            else
            {
                // std::cout << "No status\n";
            }
            if (commands_sent < commands.size())
            {
                std::cout << "Command sent: " << commands[commands_sent].dump() << std::endl;
                vslib::utils::writeJsonToMessageQueue(commands[commands_sent], write_commands_queue);
                commands_sent++;
            }
        }

        // if (commands_sent >= commands.size())
        // {
        //     break;
        // }

        uint64_t microseconds_since_epoch
            = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
                  .count();
        auto data_queue_message = read_data_queue.read(data_buffer);
        if (data_queue_message.has_value())
        {
            const std::array<float, 9> values = data_queue_message.value().first.data;

            std::cout << "received " << values[1] << " at :" << data_queue_message.value().first.clk_cycles << '\n';

            adc_output_file << data_queue_message.value().first.clk_cycles << ',' << microseconds_since_epoch << ',';
            for (int index = 0; index < 9; index++)
            {
                adc_output_file << values[index] << ",";
            }
            adc_output_file << '\n';
            if (counter++ >= 100'000)
            {
                break;
            }
        }

        // END OF TEST CODE

        // Add some delay to simulate work
        // usleep(100'000);   // 100 ms
    }
    // std::cout << "Now running the console: \n";
    // runConsoleUntilInterrupted(*domain);

    adc_output_file.close();

    return 0;
}
