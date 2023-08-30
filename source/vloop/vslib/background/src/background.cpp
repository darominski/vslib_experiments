//! @file
//! @brief File containing implementations of background task functions.
//! @author Dominik Arominski

#include <cstring>
#include <iostream>

#include "background.h"
#include "logString.h"
#include "parameter.h"
#include "parameterRegistry.h"

using namespace nlohmann;
using namespace vslib::utils;

namespace vslib::background_task
{
    //! Validates the provided json command.
    //!
    //! @return True if the command contains all expected fields, false otherwise.
    bool validateJsonCommand(const StaticJson& command)
    {
        bool valid = true;
        if (!command.contains("name"))
        {
            LogString error_msg("Command must contain 'name'.\n");
            std::cerr << error_msg;
            valid = false;
        }
        else if (!command.contains("value"))
        {
            LogString error_msg("Command must contain 'value' to be set.\n");
            std::cerr << error_msg;
            valid = false;
        }
        return valid;
    }

    //! Processes the received JSON commands, checking whether one or many commands were received.
    //! @param command JSON object containing one or more JSON commands to be executed
    void processJsonCommands(const StaticJson& commands)
    {
        if (commands.is_object())   // single command
        {
            executeJsonCommand(commands);
        }
        else if (commands.is_array())   // multiple commands
        {
            std::for_each(
                std::cbegin(commands), std::cend(commands),
                [](const auto& command)
                {
                    executeJsonCommand(command);
                }
            );
        }
    }

    //! Executes a single JSON command by moving the received command value to the memory address
    //! specified in ParameterRegistry for the received parameter name.
    //!
    //! @param command JSON object containing name of the parameter to be modified, and the new value with its type to
    //! be inserted
    void executeJsonCommand(const StaticJson& command)
    {
        if (!validateJsonCommand(command))
        {
            LogString error_msg("Command invalid, ignored.\n");
            std::cerr << error_msg;
            return;
        }
        std::string const parameter_name     = command["name"];
        auto const&       parameter_registry = parameters::ParameterRegistry::instance().getParameters();
        auto const        parameter          = parameter_registry.find(parameter_name);
        if (parameter == parameter_registry.end())
        {
            LogString error_msg("Parameter ID: " + parameter_name + " not found!\n Command ignored.\n");
            std::cerr << error_msg;
            return;
        }

        // execute the command, parameter will handle the validation of provided value.
        auto const result = (*parameter).second.get().setJsonValue(command["value"]);
        if (result.has_value())
        {
            std::cerr << result.value();
        }
        else   // success
        {
            // synchronise the write buffer with the background buffer
            (*parameter).second.get().synchroniseWriteBuffer();
        }
    }

    //! Calls each registered parameter to synchronise read buffers
    void synchroniseReadBuffers()
    {
        auto const& parameters = parameters::ParameterRegistry::instance().getParameters();
        std::for_each(
            parameters.cbegin(), parameters.cend(),
            [](const auto& parameter_entry)
            {
                parameter_entry.second.get().synchroniseReadBuffers();
            }
        );
    }

}   // namespace background_task