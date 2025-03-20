#include <array>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>

#include "bmboot/domain.hpp"
#include "bmboot/domain_helpers.hpp"
#include "json/json.hpp"
#include "messageQueue.hpp"
#include "shared_memory.h"
#include "vslibMessageQueue.hpp"

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

    static constexpr double control_period           = 50e-6;
    const double            maverage_notch_frequency = 333.33;
    static constexpr double v_base                   = 1950.0;
    static constexpr double i_base                   = 3300.0;
    static constexpr double v_max                    = 5000.0;
    static constexpr double current_frequency        = 50;   // Hz

    values_to_set["example.control_period"] = control_period;

    values_to_set["current_balancing_pos.i_base"]                   = i_base;
    values_to_set["current_balancing_pos.v_max"]                    = v_max;
    values_to_set["current_balancing_pos.maverage_notch_frequency"] = maverage_notch_frequency;
    values_to_set["current_balancing_pos.a_factors"]                = std::array<double, 3>{5.4e-3, -1.2e-3, -1.2e-3};
    values_to_set["current_balancing_pos.b_factors"]                = std::array<double, 3>{-1.2e-3, 5.4e-3, -1.2e-3};
    values_to_set["current_balancing_pos.c_factors"]                = std::array<double, 3>{-1.2e-3, -1.2e-3, 5.4e-3};
    values_to_set["current_balancing_pos.fifth_filter_order"]       = false;

    values_to_set["current_balancing_neg.i_base"]                   = i_base;
    values_to_set["current_balancing_neg.v_max"]                    = v_max;
    values_to_set["current_balancing_neg.maverage_notch_frequency"] = maverage_notch_frequency;
    values_to_set["current_balancing_neg.a_factors"]                = std::array<double, 3>{5.4e-3, -1.2e-3, -1.2e-3};
    values_to_set["current_balancing_neg.b_factors"]                = std::array<double, 3>{-1.2e-3, 5.4e-3, -1.2e-3};
    values_to_set["current_balancing_neg.c_factors"]                = std::array<double, 3>{-1.2e-3, -1.2e-3, 5.4e-3};
    values_to_set["current_balancing_neg.fifth_filter_order"]       = false;

    // zero_division and saturation
    values_to_set["avoid_zero_division.lower_threshold"] = 1e-10;
    values_to_set["avoid_zero_division.upper_threshold"] = 1e3;
    values_to_set["avoid_zero_division.dead_zone"]       = std::array<double, 2>{0, 0};

    values_to_set["saturation_protection.lower_threshold"] = -0.03;
    values_to_set["saturation_protection.upper_threshold"] = 0.03;
    values_to_set["saturation_protection.dead_zone"]       = std::array<double, 2>{0, 0};

    // POPS dispatcher
    values_to_set["dispatcher.magnets_r"]      = 0.32;
    values_to_set["dispatcher.magnets_l"]      = 0.97;
    values_to_set["dispatcher.operating_mode"] = "degraded_2";

    // PLL
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

    // outer loops
    const double kp_outer = 1.0;
    const double ki_outer = 7.5;

    values_to_set["pi_id_ref.kp"]                      = kp_outer;
    values_to_set["pi_id_ref.ki"]                      = ki_outer;
    values_to_set["pi_id_ref.kd"]                      = 0.0;
    values_to_set["pi_id_ref.kff"]                     = 0.0;
    values_to_set["pi_id_ref.proportional_scaling"]    = 1.0;
    values_to_set["pi_id_ref.derivative_scaling"]      = 1.0;
    values_to_set["pi_id_ref.derivative_filter_order"] = 1.0;
    values_to_set["pi_id_ref.control_period"]          = control_period;
    values_to_set["pi_id_ref.pre_warping_frequency"]   = 1e-9;

    values_to_set["pi_iq_ref.kp"]                      = kp_outer;
    values_to_set["pi_iq_ref.ki"]                      = ki_outer;
    values_to_set["pi_iq_ref.kd"]                      = 0.0;
    values_to_set["pi_iq_ref.kff"]                     = 0.0;
    values_to_set["pi_iq_ref.proportional_scaling"]    = 1.0;
    values_to_set["pi_iq_ref.derivative_scaling"]      = 1.0;
    values_to_set["pi_iq_ref.derivative_filter_order"] = 1.0;
    values_to_set["pi_iq_ref.control_period"]          = control_period;
    values_to_set["pi_iq_ref.pre_warping_frequency"]   = 1e-9;

    // inner loops
    const double Zb       = 1950 / (3300 / sqrt(2.0));
    const double kp_inner = (0.7e-3 / Zb) / 2.0 / control_period;
    const double ki_inner = (1e-5 / Zb) / 2.0 / control_period;

    values_to_set["pi_vd_ref.kp"]                      = kp_inner;
    values_to_set["pi_vd_ref.ki"]                      = ki_inner;
    values_to_set["pi_vd_ref.kd"]                      = 0.0;
    values_to_set["pi_vd_ref.kff"]                     = 0.0;
    values_to_set["pi_vd_ref.proportional_scaling"]    = 1.0;
    values_to_set["pi_vd_ref.derivative_scaling"]      = 1.0;
    values_to_set["pi_vd_ref.derivative_filter_order"] = 1.0;
    values_to_set["pi_vd_ref.control_period"]          = control_period;
    values_to_set["pi_vd_ref.pre_warping_frequency"]   = 1e-9;

    values_to_set["pi_vq_ref.kp"]                      = kp_inner;
    values_to_set["pi_vq_ref.ki"]                      = ki_inner;
    values_to_set["pi_vq_ref.kd"]                      = 0.0;
    values_to_set["pi_vq_ref.kff"]                     = 0.0;
    values_to_set["pi_vq_ref.proportional_scaling"]    = 1.0;
    values_to_set["pi_vq_ref.derivative_scaling"]      = 1.0;
    values_to_set["pi_vq_ref.derivative_filter_order"] = 1.0;
    values_to_set["pi_vq_ref.control_period"]          = control_period;
    values_to_set["pi_vq_ref.pre_warping_frequency"]   = 1e-12;

    // AFE parameters
    values_to_set["afe_rst.inductance"] = 0.7e-3;
    values_to_set["afe_rst.frequency"]  = current_frequency;
    values_to_set["afe_rst.v_base"]     = v_base;
    values_to_set["afe_rst.i_base"]     = i_base;

    values_to_set["example.inductance"] = 0.7e-3;
    values_to_set["example.frequency"]  = current_frequency;
    values_to_set["example.v_base"]     = v_base;
    values_to_set["example.i_base"]     = i_base;

    values_to_set["power_3ph_instant.p_gain"] = sqrt(2) / (sqrt(3) * v_base * i_base);
    values_to_set["power_3ph_instant.q_gain"] = sqrt(2) / (sqrt(3) * v_base * i_base);

    // final saturation
    values_to_set["limit.lower_threshold"] = -1.5;
    values_to_set["limit.upper_threshold"] = 1.5;
    values_to_set["limit.dead_zone"]       = std::array<double, 2>{0, 0};

    // RST outer vdc control
    values_to_set["rst_outer_vdc.r"]
        = std::array<double, 2>{2.827280412152077772454317e-8, -2.827033706921389033422873e-8};
    values_to_set["rst_outer_vdc.s"] = std::array<double, 2>{4e-8, -4e-8};
    values_to_set["rst_outer_vdc.t"] = std::array<double, 2>{2.46705230688704093725432e-12, 0.0};

    // RST outer loop control
    std::array<double, 3> r_outer{34.98137745638047846341578, -70.70286258390524380956776, 35.72193714699132982559604};
    std::array<double, 3> s_outer{1.0, -1.0, 0.0};
    std::array<double, 3> t_outer{4.5201946656448e-4, 0.0, 0.0};

    values_to_set["rst_outer_id.r"] = r_outer;
    values_to_set["rst_outer_id.s"] = s_outer;
    values_to_set["rst_outer_id.t"] = t_outer;

    values_to_set["rst_outer_iq.r"] = r_outer;
    values_to_set["rst_outer_iq.s"] = s_outer;
    values_to_set["rst_outer_iq.t"] = t_outer;

    // RST inner loop control
    std::array<double, 2> r_inner{-3.416063337264310995067262, 3.111243602756687653965173};
    std::array<double, 2> s_inner{1.0, -1.0};
    std::array<double, 2> t_inner{0.3048197345076233411020894, 0.0};

    values_to_set["rst_inner_vd.r"] = r_inner;
    values_to_set["rst_inner_vd.s"] = s_inner;
    values_to_set["rst_inner_vd.t"] = t_inner;

    values_to_set["rst_inner_vq.r"] = r_inner;
    values_to_set["rst_inner_vq.s"] = s_inner;
    values_to_set["rst_inner_vq.t"] = t_inner;

    // RST for Vdc balancing:
    values_to_set["rst_vdc.r"] = std::array<double, 2>{1.000125e-10, -9.99875e-11};
    values_to_set["rst_vdc.s"] = std::array<double, 2>{5e-8, -5e-8};
    values_to_set["rst_vdc.t"] = std::array<double, 2>{1.000125e-10, -9.99875e-11};

    // 2nd order IIR for Vdc-diff
    const double w0  = 2 * std::numbers::pi * 15;
    const double ksi = 0.85;

    const double w     = w0 * sqrt(1 - ksi * ksi);
    const double alfa  = exp(-ksi * w0 * control_period);
    const double beta  = cos(w * control_period);
    const double delta = sin(w * control_period);

    const double p_b1 = 1 - alfa * (beta + (ksi * w0 / w) * delta);
    const double p_b2 = alfa * alfa + alfa * (ksi * w0 * delta / w - beta);
    const double p_a1 = -2 * alfa * beta;
    const double p_a2 = alfa * alfa;

    values_to_set["iir_vdc.numerator_coefficients"]   = std::array<double, 3>{0.0, p_b1, p_b2};
    values_to_set["iir_vdc.denominator_coefficients"] = std::array<double, 3>{1.0, p_a1, p_a2};

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
