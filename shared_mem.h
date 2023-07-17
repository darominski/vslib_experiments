#pragma once

#include <array>
#include <variant>

#include "addressRegistry.h"
#include "pid.h"

struct SharedMemory
{
    std::array<addressRegistry::AddressStruct, addressRegistry::max_registry_size> addressList;
    int                                                                            acknowledgeCounter{0};
    int                                                                            transmissionCounter{0};
    intptr_t                                                                       commandAddress;
    std::variant<int, double, std::array<double, 4>>                               commandVal;
    size_t                                                                         commandSize;
};