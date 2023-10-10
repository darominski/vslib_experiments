//! @file
//! @brief File containing background-task specific code for copying and synchronising parameter buffers.
//! @author Dominik Arominski

#pragma once

#include "json/json.hpp"
#include "staticJson.h"

namespace vslib::backgroundTask
{
    void executeJsonCommand(const fgc4::utils::StaticJson&);
    void processJsonCommands(const fgc4::utils::StaticJson&);
    void synchroniseReadBuffers();
    bool validateJsonCommand(const fgc4::utils::StaticJson&);
}   // namespace backgroundTask
