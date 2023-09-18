//! @file
//! @brief File with constants namespace defining all constants used by the library that may be used by or be
//! communicated to the Linux core or FGCD.
//! @author Dominik Arominski

#pragma once

#include <cstdint>
#include <string>

namespace vslib::components
{
    class Component;   // fwd declaration
}

namespace vslib::constants
{
    // ************************************************************
    // Constants defined for static memory pools for supported use cases of the RingBuffer

    constexpr uint64_t json_memory_pool_size   = 1024 * 1024;   // 1 MB
    constexpr uint64_t string_memory_pool_size = 1024;          // 1024 objects of type std::string, approximately 32 kb

    // ************************************************************
    // Constant error code numbers

    // arbitrary numbers for testing purposes
    constexpr uint32_t error_json_command_invalid              = 1000;
    constexpr uint32_t error_json_missing_name                 = 1001;
    constexpr uint32_t error_json_missing_value                = 1002;
    constexpr uint32_t error_json_parameter_id_invalid         = 1003;
    constexpr uint32_t error_json_command_value_outside_limits = 1004;
    constexpr uint32_t error_json_command_value_type_invalid   = 1005;
    constexpr uint32_t error_json_command_invalid_enum_value   = 1006;

    // ************************************************************
    // Predefined component names for use in FGC Configurator

    constexpr std::string_view component_type_pid  = "PID";
    constexpr std::string_view component_type_rst  = "RST";
    constexpr std::string_view component_type_user = "User";

    // ************************************************************
    // Names of supported types for use in FGC Configurator

    constexpr std::string_view bool_type_label = "Bool";

    constexpr std::string_view float32_type_label = "Float32";
    constexpr std::string_view float64_type_label = "Float64";

    constexpr std::string_view enum_type_label = "Enum";

    constexpr std::string_view int64_type_label  = "Int64";
    constexpr std::string_view uint64_type_label = "UInt64";
    constexpr std::string_view int32_type_label  = "Int32";
    constexpr std::string_view uint32_type_label = "UInt32";
    constexpr std::string_view int16_type_label  = "Int16";
    constexpr std::string_view uint16_type_label = "UInt16";
    constexpr std::string_view int8_type_label   = "Int8";
    constexpr std::string_view uint8_type_label  = "UInt8";

    constexpr std::string_view string_type_label = "String";

    constexpr std::string_view array_type_prefix = "Array";
    // Array types have prefix "Array" and one of the above types
    // for example a parameter of type T=std::array<double, 16> will have "ArrayFloat64" label

    constexpr std::string_view unsupported_type_label = "Unsupported";

    // ************************************************************
    // Constant for denoting that the component constructed is independent (has no parents)
    constexpr components::Component* independent_component = nullptr;

}
