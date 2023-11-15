//! @file
//! @brief File containing background-task specific code triggering parameter buffer synchronisation.
//! @author Dominik Arominski

#pragma once

#include <cstring>
#include <iostream>

#include "background.h"
#include "bufferSwitch.h"
#include "componentRegistry.h"
#include "parameter.h"
#include "parameterRegistry.h"
#include "sharedMemoryVslib.h"
#include "staticJson.h"
#include "warningMessage.h"

namespace vslib::backgroundTask
{
    void initializeMemory(SharedMemory*);
    void uploadParameterMap(SharedMemory*);
    void receiveJsonCommand(SharedMemory*);
    void executeJsonCommand(const fgc4::utils::StaticJson&);
    void processJsonCommands(const fgc4::utils::StaticJson&);
    void synchroniseReadBuffers();
    bool validateJsonCommand(const fgc4::utils::StaticJson&);

    //! Connects to the initialization of the shared memory structure to a known state
    void initializeMemory(SharedMemory* shared_memory)
    {
        initializeSharedMemory(shared_memory);
    }

    //! Creates and uploads the parameter map to the shared memory. The memory is reinitialized each time
    //! this method is called.
    void uploadParameterMap(SharedMemory* shared_memory)
    {
        auto json_component_registry = fgc4::utils::StaticJsonFactory::getJsonObject();
        json_component_registry      = ComponentRegistry::instance().createParameterMap();
        writeJsonToSharedMemory(json_component_registry, shared_memory);
    }

    static bool received_new_data = false;

    //! Checks if a new command has arrived in shared memory, processes it, and when
    //! new command has come previously switches buffers and calls to synchronise them
    void receiveJsonCommand(SharedMemory* shared_memory)
    {
        if (shared_memory->transmitted_counter > shared_memory->acknowledged_counter)
        {
            auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
            json_object      = readJsonFromSharedMemory(shared_memory);
            // execute the command from the incoming stream, synchronises write and background buffers
            processJsonCommands(json_object);

            // acknowledge transaction
            shared_memory->acknowledged_counter++;
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
    //! @param command JSON object to be validated as a valid command
    //! @return True if the command contains all expected fields, false otherwise.
    bool validateJsonCommand(const fgc4::utils::StaticJson& command)
    {
        bool valid = true;
        if (!command.contains("name"))
        {
            const fgc4::utils::Warning message("Command must contain 'name'.\n");
            valid = false;
        }
        else if (!command.contains("value"))
        {
            const fgc4::utils::Warning message("Command must container 'value'.\n");
            valid = false;
        }
        return valid;
    }

    //! Processes the received JSON commands, checking whether one or many commands were received.
    //!
    //! @param command JSON object containing one or more JSON commands to be executed
    void processJsonCommands(const fgc4::utils::StaticJson& commands)
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
    void executeJsonCommand(const fgc4::utils::StaticJson& command)
    {
        if (!validateJsonCommand(command))
        {
            const fgc4::utils::Warning message("Command invalid, ignored.\n");
            return;
        }
        std::string const parameter_name     = command["name"];
        auto const&       parameter_registry = ParameterRegistry::instance().getParameters();
        auto const        parameter          = parameter_registry.find(parameter_name);
        if (parameter == parameter_registry.end())
        {
            const fgc4::utils::Warning messsage("Parameter ID: " + parameter_name + " not found. Command ignored.\n");
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

}   // namespace vslib::backgroundTask
