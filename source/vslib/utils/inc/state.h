//! @file
//! @brief Definition of VSlib generic finite state machine
//! @author Dominik Arominski

#pragma once

#include <iostream>

#include "bmboot.hpp"
#include "component.h"
#include "componentValidation.h"
#include "constants.h"
#include "converter.h"
#include "fsm.h"
#include "parameterInitialized.h"
#include "parameterMap.h"
#include "parameterRegistry.h"
#include "parameterSetting.h"
#include "vslib_shared_memory_memmap.h"

namespace vslib::utils
{
    enum class VSStates
    {
        initialization,
        unconfigured,
        configuring,
        configured,
    };

    class VSMachine
    {
        using StateMachine = ::utils::Fsm<VSStates, VSMachine, false>;

        using TransResVS = ::utils::FsmTransitionResult<VSStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<VSStates> (VSMachine::*)();

        constexpr static size_t read_commands_queue_address
            = app_data_2_3_ADDRESS;   // this needs to be CPU-choice dependent, or fixed to CPU3
        constexpr static size_t write_commands_status_queue_address
            = read_commands_queue_address + fgc4::utils::constants::json_memory_pool_size;
        constexpr static size_t write_parameter_map_queue_address = read_commands_queue_address
                                                                    + fgc4::utils::constants::json_memory_pool_size
                                                                    + fgc4::utils::constants::string_memory_pool_size;

      public:
        VSMachine(Component& root)
            : m_fsm(*this, VSStates::initialization),
              m_root(root),
              m_parameter_setting_task{
                  (uint8_t*)read_commands_queue_address, (uint8_t*)write_commands_status_queue_address, root},
              m_parameter_map{
                  (uint8_t*)write_parameter_map_queue_address, fgc4::utils::constants::json_memory_pool_size, m_root}

        {
            // CAUTION: The order of transition method matters

            // clang-format off
            m_fsm.addState(VSStates::initialization,  &VSMachine::onInitialization,  {&VSMachine::toUnconfigured});
            m_fsm.addState(VSStates::unconfigured,    &VSMachine::onUnconfigured,  {&VSMachine::toConfiguring});
            m_fsm.addState(VSStates::configuring,     &VSMachine::onConfiguring,  {&VSMachine::toUnconfigured, &VSMachine::toConfigured});
            m_fsm.addState(VSStates::configured,      &VSMachine::onConfigured,  {&VSMachine::toConfiguring});
            // clang-format on
        }

        void update()
        {
            m_fsm.update();
        }

        [[nodiscard]] const auto getState() const noexcept
        {
            return m_fsm.getState();
        }

        bool isConfigured() const
        {
            return m_fsm.getState() == VSStates::configured;
        }

        void setConverter(::vslib::IConverter* converter)
        {
            m_converter = converter;
        }

      private:
        StateMachine m_fsm;

        ::vslib::Component&       m_root;
        ::vslib::IConverter*      m_converter{nullptr};
        ::vslib::ParameterSetting m_parameter_setting_task;
        ::vslib::ParameterMap     m_parameter_map;

        void onInitialization()
        {
            bmboot::notifyPayloadStarted();
            // everything generic that needs to be done to initialize the vloop
        }

        void onUnconfigured()
        {
            // upload the Parameter map so that GUI can be built based on it and Parameters can be eventually set
            m_parameter_map.uploadParameterMap();
        }

        void onConfiguring()
        {
            // receive and process commands
            m_parameter_setting_task.receiveJsonCommand();
            // when done, transition away
            m_fsm.update();
        }

        void onConfigured()
        {
            // initialize user code (RT)
            m_converter->init();
            // background task running continuously
            while (true)
            {

                if (m_parameter_setting_task.checkNewSettingsAvailable())
                {
                    // if new settings are available, transition to configuring
                    m_fsm.update();
                }
                // other background tasks
                // ...
                //

                // user background task:
                m_converter->backgroundTask();

                // for testing purposes:
                break;
            }
        }

        TransResVS toConfiguring()
        {
            return {VSStates::configuring};
        }

        TransResVS toUnconfigured()
        {
            // allow transition if all Parameters have been initialized
            if (!vslib::utils::parametersInitialized())
            {
                return {VSStates::unconfigured};
            }
            // do not transition otherwise
            return {};
        }

        TransResVS toConfigured()
        {
            // allow transition if all Parameters have been initialized
            if (vslib::utils::parametersInitialized())
            {
                return {VSStates::configured};
            }
            // do not transition otherwise
            return {};
        }
    };

}   // namespace vslib::utils