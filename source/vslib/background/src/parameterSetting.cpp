//! @file
//! @brief Source file containing library-side background task code for receiving and
//! validation of incoming commands, their execution, and triggering synchronisation of buffers.
//! @author Dominik Arominski

#include "bufferSwitch.h"
#include "constants.h"
#include "errorCodes.h"
#include "fmt/format.h"
#include "parameter.h"
#include "parameterRegistry.h"
#include "parameterSetting.h"
#include "versions.h"
#include "warningMessage.h"

using namespace fgc4::utils;

namespace vslib
{

    //! Checks if a new command has arrived in shared memory, processes it, and when
    //! new command has come previously switches buffers and calls to synchronise them
    void ParameterSetting::receiveJsonCommand()
    {
        auto message = m_read_commands_queue.read(m_read_commands_buffer);
        if (message.has_value())
        {
            auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
            json_object      = utils::readJsonFromMessageQueue(message.value());
            // execute the command from the incoming stream, synchronises write and background buffers
            processJsonCommands(json_object);

            // after the processing, validate all touched Components
            auto const maybe_error = validateModifiedComponents();

            // TO-DO: wait a little in case more commands come before flipping state
            if (maybe_error.has_value())
            {
                BufferSwitch::flipState();   // flip the buffer pointer of all settable parameters
                // synchronise new background to new active buffer
                triggerReadBufferSynchronisation();
            }
            // else: message already logged, buffers will not be synchronised and state flipped
        }
    }

    //! Processes the received JSON commands, checking whether one or many commands were received.
    //!
    //! @param command JSON object containing one or more JSON commands to be executed
    void ParameterSetting::processJsonCommands(const fgc4::utils::StaticJson& commands)
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
    bool ParameterSetting::validateJsonCommand(const fgc4::utils::StaticJson& command)
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
            utils::writeStringToMessageQueue(message.warning_str.data(), m_write_command_status);
        }
        // check that major version is consistent
        if (valid)
        {
            // The try-catch avoids fatal failure in case version is severely malformed: e.g. if it is not a list,
            // or contains elements not comparable with integers
            try
            {
                valid = (command["version"][0] == vslib::version::json_command.major);
            }
            catch (const std::exception& e)
            {
                valid = false;
                const fgc4::utils::Warning message(std::string("Command invalid: ") + e.what());
                utils::writeStringToMessageQueue(message.warning_str.data(), m_write_command_status);
                return valid;
            }
            if (!valid)
            {
                const fgc4::utils::Warning message(fmt::format(
                    "Inconsistent major version of the communication interface! Provided version: {}, expected "
                    "version: {}.\n",
                    command["version"][0], vslib::version::json_command.major
                ));
                utils::writeStringToMessageQueue(message.warning_str.data(), m_write_command_status);
            }
        }
        return valid;
    }

    //! Executes a single JSON command by setting the received command value to the parameter reference
    //! stored in ParameterRegistry identified by the command's parameter name.
    //!
    //! @param command JSON object containing name of the parameter to be modified, and the new value with its type to
    //! be inserted
    void ParameterSetting::executeJsonCommand(const fgc4::utils::StaticJson& command)
    {
        if (!validateJsonCommand(command))
        {
            const fgc4::utils::Warning message("Command invalid, ignored.\n");
            return;
        }
        const std::string& parameter_name     = command["name"];
        const auto&        parameter_registry = ParameterRegistry::instance().getParameters();
        const auto&        parameter          = parameter_registry.find(parameter_name);
        if (parameter == parameter_registry.end())
        {
            const fgc4::utils::Warning message("Parameter ID: " + parameter_name + " not found. Command ignored.\n");
            utils::writeStringToMessageQueue(message.warning_str.data(), m_write_command_status);
            return;
        }

        // execute the command, parameter will handle the validation of provided value.
        auto const has_warning = (*parameter).second.get().setJsonValue(command["value"]);
        if (!has_warning.has_value())
        {
            // success, otherwise: failure and Warning message already logged by setJsonValue
            // synchronise the write buffer with the background buffer
            (*parameter).second.get().synchroniseWriteBuffer();
            utils::writeStringToMessageQueue("Parameter value updated successfully.\n", m_write_command_status);

            // TODO: do we need to mark children of the modified component?
        }
        else
        {
            utils::writeStringToMessageQueue(has_warning.value().warning_str.data(), m_write_command_status);
        }
    }

    //! Calls verifyParameters of all modified components in the registry
    //!
    //! @return Optionally returns a Warning if validation has failed.
    std::optional<fgc4::utils::Warning> ParameterSetting::validateModifiedComponents()
    {
        auto const& component_registry = ComponentRegistry::instance().getComponents();
        for (const auto& entry : component_registry)
        {
            auto& component = entry.second.get();
            if (component.parametersModified())
            {
                auto const maybe_warning = component.verifyParameters();
                if (maybe_warning.has_value())
                {
                    // validation did not pass, roll back background buffer update and return a warning
                    for (auto& parameter : component.getParameters())
                    {
                        parameter.second.get().synchroniseReadBuffers();
                    }
                    return maybe_warning.value();
                }
                component.setParametersModified(false);
            }
        }
        return {};
    }

    //! Calls each registered parameter to synchronise background with real-time buffers
    void ParameterSetting::triggerReadBufferSynchronisation()
    {
        for (const auto& parameter : ParameterRegistry::instance().getParameters())
        {
            parameter.second.get().synchroniseReadBuffers();
        }
    }
}   // namespace vslib
