#include <array>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>

#include "bmboot/domain.hpp"
#include "bmboot/domain_helpers.hpp"
#include "json/json.hpp"
#include "messageQueue.h"
#include "shared_memory.h"
#include "vslibMessageQueue.h"

using Json = nlohmann::json;
using namespace vslib;
using namespace fgcd;
using namespace fgc4;

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

    const double control_period = 1e-3;

    values_to_set["example.control_period"] = control_period;

    // POPS dispatcher
    values_to_set["dispatcher.magnets_r"]      = 0.32;
    values_to_set["dispatcher.magnets_l"]      = 0.97;
    values_to_set["dispatcher.operating_mode"] = "degraded_2";

    // // PLL
    // values_to_set["pll.f_rated"]      = 50.0;
    // values_to_set["pll.angle_offset"] = 0.0;

    // values_to_set["pi.kp"]                      = 400.0;
    // values_to_set["pi.ki"]                      = 35000.0;
    // values_to_set["pi.kd"]                      = 0.0;
    // values_to_set["pi.kff"]                     = 0.0;
    // values_to_set["pi.proportional_scaling"]    = 1.0;
    // values_to_set["pi.derivative_scaling"]      = 1.0;
    // values_to_set["pi.derivative_filter_order"] = 1.0;
    // values_to_set["pi.control_period"]          = control_period;
    // values_to_set["pi.pre_warping_frequency"]   = 1e-9;

    // values_to_set["actuation_limits.lower_threshold"] = -1e9;
    // values_to_set["actuation_limits.upper_threshold"] = 1e9;
    // values_to_set["actuation_limits.dead_zone"]       = std::array<double, 2>{0, 0};

    // // outer loops
    // const double kp_outer = 1.0;
    // const double ki_outer = 7.5;

    // values_to_set["pi_id_ref.kp"]                      = kp_outer;
    // values_to_set["pi_id_ref.ki"]                      = ki_outer;
    // values_to_set["pi_id_ref.kd"]                      = 0.0;
    // values_to_set["pi_id_ref.kff"]                     = 0.0;
    // values_to_set["pi_id_ref.proportional_scaling"]    = 1.0;
    // values_to_set["pi_id_ref.derivative_scaling"]      = 1.0;
    // values_to_set["pi_id_ref.derivative_filter_order"] = 1.0;
    // values_to_set["pi_id_ref.control_period"]          = control_period;
    // values_to_set["pi_id_ref.pre_warping_frequency"]   = 1e-9;

    // values_to_set["pi_iq_ref.kp"]                      = kp_outer;
    // values_to_set["pi_iq_ref.ki"]                      = ki_outer;
    // values_to_set["pi_iq_ref.kd"]                      = 0.0;
    // values_to_set["pi_iq_ref.kff"]                     = 0.0;
    // values_to_set["pi_iq_ref.proportional_scaling"]    = 1.0;
    // values_to_set["pi_iq_ref.derivative_scaling"]      = 1.0;
    // values_to_set["pi_iq_ref.derivative_filter_order"] = 1.0;
    // values_to_set["pi_iq_ref.control_period"]          = control_period;
    // values_to_set["pi_iq_ref.pre_warping_frequency"]   = 1e-9;

    // // inner loops
    // const double Zb       = 1950 / (3300 / sqrt(2.0));
    // const double kp_inner = (0.7e-3 / Zb) / 2.0 / control_period;
    // const double ki_inner = (1e-5 / Zb) / 2.0 / control_period;

    // values_to_set["pi_vd_ref.kp"]                      = kp_inner;
    // values_to_set["pi_vd_ref.ki"]                      = ki_inner;
    // values_to_set["pi_vd_ref.kd"]                      = 0.0;
    // values_to_set["pi_vd_ref.kff"]                     = 0.0;
    // values_to_set["pi_vd_ref.proportional_scaling"]    = 1.0;
    // values_to_set["pi_vd_ref.derivative_scaling"]      = 1.0;
    // values_to_set["pi_vd_ref.derivative_filter_order"] = 1.0;
    // values_to_set["pi_vd_ref.control_period"]          = control_period;
    // values_to_set["pi_vd_ref.pre_warping_frequency"]   = 1e-9;

    // values_to_set["pi_vq_ref.kp"]                      = kp_inner;
    // values_to_set["pi_vq_ref.ki"]                      = ki_inner;
    // values_to_set["pi_vq_ref.kd"]                      = 0.0;
    // values_to_set["pi_vq_ref.kff"]                     = 0.0;
    // values_to_set["pi_vq_ref.proportional_scaling"]    = 1.0;
    // values_to_set["pi_vq_ref.derivative_scaling"]      = 1.0;
    // values_to_set["pi_vq_ref.derivative_filter_order"] = 1.0;
    // values_to_set["pi_vq_ref.control_period"]          = control_period;
    // values_to_set["pi_vq_ref.pre_warping_frequency"]   = 1e-9;

    // // final saturation
    // values_to_set["limit.lower_threshold"] = -1.1;
    // values_to_set["limit.upper_threshold"] = 1.1;
    // values_to_set["limit.dead_zone"]       = std::array<double, 2>{0, 0};

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

    auto write_commands_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueWriter<void>>(
        (uint8_t*)buffer.data(), json_queue_size
    );

    auto read_command_status_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)buffer.data() + json_queue_size, string_queue_size
    );

    auto read_parameter_map_queue = fgc4::utils::createMessageQueue<fgc4::utils::MessageQueueReader<void>>(
        (uint8_t*)buffer.data() + json_queue_size + string_queue_size, json_queue_size
    );

    fprintf(stderr, "Run payload\n");
    bmboot::loadPayloadFromFileOrThrow(*domain, argv[1]);
    usleep(500'000);   // delay for initialization

    std::array<uint8_t, json_queue_size>   parameter_map_buffer;
    std::array<uint8_t, string_queue_size> command_status_buffer{0};
    std::vector<Json>                      commands;
    bool                                   commands_set  = false;
    size_t                                 counter       = 0;
    size_t                                 commands_sent = 0;
    while (true)
    {
        std::cout << "Linux counter: " << counter++ << "\n";
        // TEST CODE FOR TRANSFERRING COMMANDS
        // there are 3 PID with 9 params + RST with 1 parameter, so 10 in total,
        // modulo prevents setting not used fields
        auto message = read_parameter_map_queue.read(parameter_map_buffer);
        if (message.has_value())
        {
            auto const json_manifest       = vslib::utils::readJsonFromMessageQueue(message.value());
            // std::cout << json_manifest.dump(1) << "\n";
            auto const settable_parameters = parseManifest(json_manifest);
            commands                       = prepareCommands(settable_parameters);
            commands_set                   = true;
        }

        if (commands_set && (commands_sent <= commands.size()))
        {
            // check status of previous command
            auto message = read_command_status_queue.read(command_status_buffer);
            if (message.has_value())
            {

                std::cout << "Status: " << std::string(message->begin(), message->end()) << std::endl;
            }
            else
            {
                std::cout << "No status\n";
            }
            if (commands_sent < commands.size())
            {
                std::cout << "Command sent: " << commands[commands_sent].dump() << std::endl;
                vslib::utils::writeJsonToMessageQueue(commands[commands_sent], write_commands_queue);
                commands_sent++;
            }
        }

        if (commands_sent >= commands.size())
        {
            break;
        }

        // END OF TEST CODE

        // Add some delay to simulate work
        usleep(100'000);   // 100 ms
    }
    std::cout << "Now running the console: \n";
    runConsoleUntilInterrupted(*domain);

    return 0;
}
