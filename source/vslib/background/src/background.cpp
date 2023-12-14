//! @file
//! @brief Source file containing library-side background task specific code for creating and uploading the parameter
//! map, validation of incoming commands, executing them, and triggering synchronisation of buffers.
//! @author Dominik Arominski

#include "background.h"
#include "bufferSwitch.h"
#include "parameter.h"
#include "parameterRegistry.h"
#include "warningMessage.h"

using namespace fgc4::utils;

namespace vslib
{
    void BackgroundTask::initializeSharedMemory()
    {
        vslib::initializeSharedMemory(m_shared_memory_ref);
    }

    //! Creates and uploads the parameter map to the shared memory. The memory is reinitialized each time
    //! this method is called.
    void BackgroundTask::uploadParameterMap()
    {
        auto json_component_registry = fgc4::utils::StaticJsonFactory::getJsonObject();
        json_component_registry      = ComponentRegistry::instance().createParameterMap();
        writeJsonToSharedMemory(json_component_registry, m_shared_memory_ref);
    }

    //! Checks if a new command has arrived in shared memory, processes it, and when
    //! new command has come previously switches buffers and calls to synchronise them
    void BackgroundTask::receiveJsonCommand()
    {
        if (m_shared_memory_ref.transmitted_counter > m_shared_memory_ref.acknowledged_counter)
        {
            auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
            json_object      = readJsonFromSharedMemory(m_shared_memory_ref);
            // execute the command from the incoming stream, synchronises write and background buffers
            processJsonCommands(json_object);

            // acknowledge transaction
            m_shared_memory_ref.acknowledged_counter++;
            m_received_new_data = true;
        }
        else if (m_received_new_data)
        {
            // if no new data came in the previous iteration, assume it is safe to switch the read buffers now and
            // synchronise them
            BufferSwitch::flipState();   // flip the buffer pointer of all settable parameters
            // synchronise new background to new active buffer
            triggerReadBufferSynchronisation();
            m_received_new_data = false;   // buffers updated, no new data available
        }
    }

    //! Processes the received JSON commands, checking whether one or many commands were received.
    //!
    //! @param command JSON object containing one or more JSON commands to be executed
    void BackgroundTask::processJsonCommands(const fgc4::utils::StaticJson& commands)
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

    //! Validates the provided json command.
    //!
    //! @param command JSON object to be validated as a valid command
    //! @return True if the command contains all expected fields, false otherwise.
    bool BackgroundTask::validateJsonCommand(const fgc4::utils::StaticJson& command)
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

    //! Executes a single JSON command by moving the received command value to the memory address
    //! specified in ParameterRegistry for the received parameter name.
    //!
    //! @param command JSON object containing name of the parameter to be modified, and the new value with its type to
    //! be inserted
    void BackgroundTask::executeJsonCommand(const fgc4::utils::StaticJson& command)
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
            const fgc4::utils::Warning message("Parameter ID: " + parameter_name + " not found. Command ignored.\n");
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
    void BackgroundTask::triggerReadBufferSynchronisation()
    {
        for (const auto& parameter : ParameterRegistry::instance().getParameters())
        {
            parameter.second.get().synchroniseReadBuffers();
        }
    }
}   // namespace backgroundTask
