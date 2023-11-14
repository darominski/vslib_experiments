//! @file
//! @brief File containing implementations of background task functions.
//! @author Dominik Arominski

#include <cstring>
#include <iostream>

#include "background.h"
#include "bufferSwitch.h"
#include "parameter.h"
#include "parameterRegistry.h"
#include "sharedMemoryVslib.h"
#include "warningMessage.h"

using namespace fgc4::utils;

namespace vslib::backgroundTask
{
    //! Creates and uploads the component and parameter map to the shared memory (and standard output)
    void uploadParameterMap()
    {
        auto json_component_registry = StaticJsonFactory::getJsonObject();
        json_component_registry      = ComponentRegistry::instance().createParameterMap();
        std::cout << json_component_registry.dump() << "\n";
        writeJsonToSharedMemory(json_component_registry, &(SHARED_MEMORY));
    }

    static bool received_new_data = false;

    //! Checks if a new command has arrived in shared memory, processes it, and when
    //! new command has come previously switches buffers and calls to synchronise them
    void receiveJsonCommand()
    {
        if (SHARED_MEMORY.transmitted_counter > SHARED_MEMORY.acknowledged_counter)
        {
            auto json_object = StaticJsonFactory::getJsonObject();
            json_object      = readJsonFromSharedMemory(&(SHARED_MEMORY));
            // execute the command from the incoming stream, synchronises write and background buffers
            processJsonCommands(json_object);

            // acknowledge transaction
            SHARED_MEMORY.acknowledged_counter++;
            received_new_data = true;
        }
        else if (received_new_data)
        {
            // if no new data came in the previous iteration, assume it is safe to switch the read buffers now and
            // synchronise them
            BufferSwitch::flipState();   // flip the buffer pointer of all settable parameters
            // synchronise new background to new active buffer
            synchroniseReadBuffers();
            received_new_data = false;   // buffers updated, no new data available
        }
    }

    //! Validates the provided json command.
    //!
    //! @return True if the command contains all expected fields, false otherwise.
    bool validateJsonCommand(const StaticJson& command)
    {
        bool valid = true;
        if (!command.contains("name"))
        {
            const Warning message("Command must contain 'name'.\n");
            valid = false;
        }
        else if (!command.contains("value"))
        {
            const Warning message("Command must container 'value'.\n");
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
            for (const auto& command : commands)
            {
                executeJsonCommand(command);
            }
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
            const Warning messsage("Command invalid, ignored.\n");
            return;
        }
        std::string const parameter_name     = command["name"];
        auto const&       parameter_registry = ParameterRegistry::instance().getParameters();
        auto const        parameter          = parameter_registry.find(parameter_name);
        if (parameter == parameter_registry.end())
        {
            const Warning messsage("Parameter ID: " + parameter_name + " not found. Command ignored.\n");
            return;
        }

        // execute the command, parameter will handle the validation of provided value.
        auto const result = (*parameter).second.get().setJsonValue(command["value"]);
        if (!result.has_value())   // success, otherwise: Warning message already logged
        {
            // synchronise the write buffer with the background buffer
            (*parameter).second.get().synchroniseWriteBuffer();
        }
    }

    //! Calls each registered parameter to synchronise read buffers
    void synchroniseReadBuffers()
    {
        for (const auto& parameter : ParameterRegistry::instance().getParameters())
        {
            parameter.second.get().synchroniseReadBuffers();
        }
    }

}   // namespace backgroundTask
