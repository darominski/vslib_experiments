//! @file
//! @brief File with definitions of Error message structure and a template specialization for its formatting
//! with fmt library.
//! @author Dominik Arominski

#pragma once

#include <fmt/format.h>
#include <string>
#include <utility>

#include "logString.hpp"

namespace fgc4::utils
{
    struct Error
    {
        Error(std::string_view _error_msg, std::size_t _error_code)
            : error_str{_error_msg},
              error_code{_error_code}
        {
            std::cerr << fmt::format("Error [{}]: {}", error_code, error_str);
        }

        explicit Error(std::string_view error_msg)
            : Error{error_msg, 0U}
        {
        }

        LogString   error_str;
        std::size_t error_code;
    };
}   // namespace fgc4::utils

// ************************************************************
// Convenience formatter for the Error class

template<>
struct fmt::formatter<fgc4::utils::Error>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }
    template<typename FormatContext>
    auto format(const fgc4::utils::Error& error, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "Error [{}]: {}", error.error_code, error.error_str);
    }
};