//! @file
//! @brief File containing the schema for commands in VSlib parameter setting over the shared memory.
//! @author Dominik Arominski

#pragma once

#include "json/json.hpp"

namespace vslib::utils
{

    // The schema is defined based upon a string literal
    static nlohmann::json json_command_schema = R"(
    {
    "title": "Command",
    "description": "Command with a new value to be set",
    "properties": {
        "name": {
            "description": "Unique string identifier for a parameter",
            "type": "string",
            "minLength": 1
        },
        "value": {
            "description": "New value to be set to the parameter",
            "type": ["array", "boolean", "number", "string"]
        }
    },
    "required": [
                 "name",
                 "value"
                 ],
    "type": "object"
}

)"_json;

}   // namespace vslib::utils
