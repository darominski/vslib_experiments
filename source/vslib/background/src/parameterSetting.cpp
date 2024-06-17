//! @file
//! @brief Source file containing library-side background task code for receiving and
//! validation of incoming commands, their execution, and triggering synchronisation of buffers.
//! @author Dominik Arominski

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

    void ParameterSetting::receiveJsonCommand()
    {
        auto message = m_read_commands_queue.read(m_read_commands_buffer);
        if (message.has_value())
        {
            auto json_object = fgc4::utils::StaticJsonFactory::getJsonObject();
            json_object      = utils::readJsonFromMessageQueue(message.value());
            // execute the command from the incoming stream, synchronises write and background buffers
            processJsonCommands(json_object);

            // after the processing, validate all Components
            validateComponents();
        }
    }

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
                const fgc4::utils::Warning message(fmt::format("Command invalid: {}", e.what()));
                utils::writeStringToMessageQueue(message.warning_str.data(), m_write_command_status);
                return valid;
            }
            if (!valid)
            {
                const int&           version = command["version"][0];
                fgc4::utils::Warning message(fmt::format(
                    "Inconsistent major version of the communication interface! Provided version: {}, expected "
                    "version: {}.\n",
                    version, vslib::version::json_command.major
                ));
                utils::writeStringToMessageQueue(message.warning_str.data(), m_write_command_status);
            }
        }
        return valid;
    }

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
        const auto& warning = (*parameter).second.get().setJsonValue(command["value"]);
        if (!warning.has_value())
        {
            // success, otherwise: failure and Warning message already logged by setJsonValue
            utils::writeStringToMessageQueue("Parameter value updated successfully.\n", m_write_command_status);
        }
        else
        {
            utils::writeStringToMessageQueue(warning.value().warning_str.data(), m_write_command_status);
        }
    }

    void ParameterSetting::validateComponents()
    {
        // validate the root Component first:
        const auto& root_warning = m_root_component.verifyParameters();
        if (!root_warning.has_value())
        {
            m_root_component.flipBufferState();
        }

        // validate all children and their children tree indefinitely deeply
        validateComponent(m_root_component.getChildren());
    }

    void ParameterSetting::validateComponent(const ChildrenList& children)
    {
        for (const auto& child : children)
        {
            auto& component = child.get();
            if (component.parametersInitialized())
            {
                const auto& warning = component.verifyParameters();
                if (!warning.has_value())
                {
                    component.flipBufferState();
                }
                // if there is an issue: it is logged, the component's buffer is not flipped
                component.synchroniseParameterBuffers();
            }
            validateComponent(component.getChildren());
        }
    }
}   // namespace vslib
