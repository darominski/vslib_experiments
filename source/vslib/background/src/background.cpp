//! @file
//! @brief Source file containing library-side background task specific code for creating and uploading the parameter
//! map, validation of incoming commands, executing them, and triggering synchronisation of buffers.
//! @author Dominik Arominski

#include "background.h"
#include "bufferSwitch.h"
#include "constants.h"
#include "errorCodes.h"
#include "fmt/format.h"
#include "parameter.h"
#include "parameterRegistry.h"
#include "versions.h"
#include "warningMessage.h"

using namespace fgc4::utils;

namespace vslib
{

    //! Creates and uploads the parameter map to the shared memory. The memory is reinitialized each time
    //! this method is called.
    void BackgroundTask::uploadParameterMap()
    {
        auto json_component_registry = fgc4::utils::StaticJsonFactory::getJsonObject();
        json_component_registry      = ComponentRegistry::instance().createParameterMap();
        writeJsonToMessageQueue(json_component_registry, m_write_parameter_map_queue);
    }

    //! Checks if a new command has arrived in shared memory, processes it, and when
    //! new command has come previously switches buffers and calls to synchronise them
    void BackgroundTask::receiveJsonCommand()
    {
        auto message = m_read_commands_queue.read(m_read_commands_buffer);

        if (message.has_value())
        {
            auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
            json_object      = readJsonFromMessageQueue(message.value());
            // execute the command from the incoming stream, synchronises write and background buffers
            processJsonCommands(json_object);

            // TO-DO: wait a little in case more commands come before flipping state

            BufferSwitch::flipState();   // flip the buffer pointer of all settable parameters
            // synchronise new background to new active buffer
            triggerReadBufferSynchronisation();
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
        try
        {
            m_validator.validate(command);
        }
        catch (const std::exception& e)
        {
            valid = false;
            const fgc4::utils::Warning message(std::string("Command invalid: ") + e.what());
        }
        // check that major version is consistent
        if (valid)
        {
            valid = (command["version"][0] == vslib::version::json_command.major);
            if (!valid)
            {
                const fgc4::utils::Warning message(fmt::format(
                    "Inconsistent major version of the communication interface! Provided version: {}, expected "
                    "version: "
                    "{}.\n",
                    command["version"][0], vslib::version::json_command.major
                ));
            }
        }
        return valid;
    }

    //! Executes a single JSON command by setting the received command value to the parameter reference
    //! stored in ParameterRegistry identified by the command's parameter name.
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
        if (!result.has_value())   // success, otherwise: failure and Warning message already logged
        {
            // synchronise the write buffer with the background buffer
            (*parameter).second.get().synchroniseWriteBuffer();
        }
    }

    //! Calls each registered parameter to synchronise background with real-time buffers
    void BackgroundTask::triggerReadBufferSynchronisation()
    {
        for (const auto& parameter : ParameterRegistry::instance().getParameters())
        {
            parameter.second.get().synchroniseReadBuffers();
        }
    }
}   // namespace backgroundTask
