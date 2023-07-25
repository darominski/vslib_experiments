//! @file
//! @brief File containing background-task specific code for copying and synchronising parameter buffers.
//! @author Dominik Arominski

#pragma once

#include "nlohmann/json.hpp"

namespace backgroundTask
{
    void synchroniseReadBuffers();
    void copyWriteBuffer();
    void executeJsonCommand(nlohmann::json);
}   // namespace backgroundTask