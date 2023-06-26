#pragma once

#include "pid.h"

struct SharedMem
{
    std::array<AddressStruct, addressRegisterSize> addrRegister;
    int                                            acknowledgeCntr{0};
    int                                            transmissionCntr{0};
    intptr_t                                       commandAddr;
    std::variant<int, double>                      commandVal;
    size_t                                         commandSize;
};