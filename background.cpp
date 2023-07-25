
#include <cstring>

#include "background.h"
#include "parameterRegistry.h"

using json = nlohmann::json;

namespace backgroundTask
{
    //! Copies all contents of the currently used buffer to the background buffer to synchronise them.
    void synchroniseReadBuffers()
    {
        auto const buffers = parameters::ParameterRegistry::instance().getBuffers();
        std::for_each(
            std::cbegin(buffers), std::cend(buffers),
            [&](auto const& map_element)
            {
                auto const first_buffer_address  = std::get<0>(map_element.second).memory_address;
                auto const second_buffer_address = std::get<1>(map_element.second).memory_address;
                ;
                auto const memory_size
                    = std::get<1>(map_element.second).memory_size;   // can be either of available buffers
                auto active_buffer     = first_buffer_address;
                auto background_buffer = second_buffer_address;
                if (buffer_switch == 1)
                {
                    std::swap(active_buffer, background_buffer);
                }
                // always copy from active buffer to background
                memcpy(reinterpret_cast<void*>(background_buffer), reinterpret_cast<void*>(active_buffer), memory_size);
            }
        );
    }

    // ************************************************************

    //! Copies all contents of a write buffer to the background buffer, which is not currently used.
    void copyWriteBuffer()
    {
        auto const buffers = parameters::ParameterRegistry::instance().getBuffers();
        std::for_each(
            std::cbegin(buffers), std::cend(buffers),
            [&](auto const& map_element)
            {
                auto const first_buffer_address  = std::get<0>(map_element.second).memory_address;
                auto const second_buffer_address = std::get<1>(map_element.second).memory_address;
                auto const write_buffer_address  = std::get<2>(map_element.second).memory_address;
                auto const memory_size
                    = std::get<1>(map_element.second).memory_size;   // can be either of available buffers
                // buffer_switch 0: real-time buffer = [0], background buffer = [1]; buffer_switch 1: [1] and [0]
                auto const background_buffer_address
                    = (buffer_switch == 0) ? second_buffer_address : first_buffer_address;
                // always copy from write to current background buffer
                memcpy(
                    reinterpret_cast<void*>(background_buffer_address), reinterpret_cast<void*>(write_buffer_address),
                    memory_size
                );
            }
        );
    }

    // ************************************************************

    void executeJsonCommand(json command)
    {
        auto              parameter_registry = parameters::ParameterRegistry::instance().getBuffers();
        std::string const parameter_name     = command["param_id"];
        auto const        parameter          = parameter_registry.find(parameter_name);
        if (parameter == parameter_registry.end())
        {
            std::cerr << "Parameter ID: " << parameter_name << " not found! Command ignored.\n";
            return;
        }
        auto const             address = std::get<2>(parameter_registry[parameter_name]).memory_address;
        const parameters::Type type    = parameters::fromString(std::string(command["type"]));
        if (type == parameters::Type::Float32)
        {
            double value = static_cast<double>(command["value"]);
            memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
        }
    }
}   // namespace backgroundTask