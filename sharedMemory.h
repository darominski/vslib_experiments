#pragma once

#include <array>
#include <variant>

#include "parameterRegistry.h"
#include "pid.h"

struct SharedMemory
{
    std::array<parameters::AddressEntry, parameters::max_registry_size> address_list;
    int                                                                 acknowledge_counter{0};
    int                                                                 transmission_counter{0};
    intptr_t                                                            command_address;
    std::variant<int, double, std::array<double, 4>>                    command_value;
    size_t                                                              command_size;
};