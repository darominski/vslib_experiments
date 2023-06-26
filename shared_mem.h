#pragma once

#include "pid.h"

struct SharedMem
{
    std::array<AddressStruct, addressRegisterSize> addrRegister;
    int                                            acknowledgeCntr{0};
    int                                            transmissionCntr{0};
    std::variant<int*, double*>                    commandAddr;
    std::variant<int, double>                      commandVal;
    size_t                                         commandSize;
};