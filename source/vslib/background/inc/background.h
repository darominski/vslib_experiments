//! @file
//! @brief File containing background-task specific code triggering parameter buffer synchronisation.
//! @author Dominik Arominski

#pragma once

#include "json/json.hpp"
#include "staticJson.h"

namespace vslib::backgroundTask
{
    void initializeSharedMemory();
    void uploadParameterMap();
    void receiveJsonCommand();
    void executeJsonCommand(const fgc4::utils::StaticJson&);
    void processJsonCommands(const fgc4::utils::StaticJson&);
    void synchroniseReadBuffers();
    bool validateJsonCommand(const fgc4::utils::StaticJson&);
}   // namespace vslib::backgroundTask
