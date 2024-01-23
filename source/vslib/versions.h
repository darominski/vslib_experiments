//! @file
//! @brief File containing version objects for all used communication interfaces.
//! @author Dominik Arominski

#pragma once

#include "version.h"

namespace vslib::version
{
    constexpr fgc4::utils::Version json_command(0, 1, 0);
    constexpr fgc4::utils::Version json_parameter_map(0, 1, 0);
}
