//! @file
//! @brief File with definitions of Warning message structure and a template specialization for its formatting
//! with fmt library.
//! @author Dominik Arominski

#pragma once

#include <fmt/format.h>
#include <string>
#include <utility>

#include "logString.hpp"

namespace fgc4::utils
{
    struct Warning
    {
        Warning(std::string_view _warning_message)
            : warning_str{_warning_message}
        {
            std::cerr << fmt::format("Warning: {}", warning_str);
        }

        LogString warning_str;
    };
}   // namespace fgc4::utils

// ************************************************************
// Convenience formatter for the Warning class

template<>
struct fmt::formatter<fgc4::utils::Warning>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }
    template<typename FormatContext>
    auto format(const fgc4::utils::Warning& warning, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "Warning: {}", warning.warning_str);
    }
};