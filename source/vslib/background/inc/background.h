//! @file
//! @brief File containing background-task specific code for copying and synchronising parameter buffers.
//! @author Dominik Arominski

#pragma once

#include "nlohmann/json.hpp"
#include "staticJson.h"

namespace vslib::backgroundTask
{
    void executeJsonCommand(const utils::StaticJson&);
    void processJsonCommands(const utils::StaticJson&);
    void synchroniseReadBuffers();
    bool validateJsonCommand(const utils::StaticJson&);
}   // namespace backgroundTask
