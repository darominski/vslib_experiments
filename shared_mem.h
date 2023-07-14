#pragma once

#include <array>
#include <variant>

#include "addressRegistry.h"
#include "pid.h"

struct SharedMem
{
    std::array<addressRegistry::AddressStruct, addressRegistry::max_registry_size> addrRegistry;
    int                                                                            acknowledgeCntr{0};
    int                                                                            transmissionCntr{0};
    intptr_t                                                                       commandAddr;
    std::variant<int, double, std::array<double, 4>>                               commandVal;
    size_t                                                                         commandSize;
};