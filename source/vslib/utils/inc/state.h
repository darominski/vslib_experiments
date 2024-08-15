//! @file
//! @brief Definition of VSlib generic finite state machine
//! @author Dominik Arominski

#pragma once

#include "component.h"
#include "constants.h"
#include "converter.h"
#include "fsm.h"
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
        VSMachine(RootComponent& root, IConverter& converter)
            : m_fsm(*this, VSStates::initialization),
              m_root(root),
              m_converter(converter),
              m_parameter_setting_task{
                  (uint8_t*)read_commands_queue_address, (uint8_t*)write_commands_status_queue_address, root},
              m_parameter_map{
                  (uint8_t*)write_parameter_map_queue_address, fgc4::utils::constants::json_memory_pool_size, root}

        {
            // CAUTION: The order of transition method matters

            // clang-format off
            m_fsm.addState(VSStates::initialization,  &VSMachine::onInitialization,  {&VSMachine::toUnconfiguredFromInit});
            m_fsm.addState(VSStates::unconfigured,    &VSMachine::onUnconfigured,    {&VSMachine::toConfiguring, &VSMachine::toConfigured});
            m_fsm.addState(VSStates::configuring,     &VSMachine::onConfiguring,     {&VSMachine::toUnconfigured, &VSMachine::toConfigured});
            m_fsm.addState(VSStates::configured,      &VSMachine::onConfigured,      {&VSMachine::toConfiguring});
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

      private:
        StateMachine m_fsm;

        bool m_init_done{false};
        bool m_first{true};

        ::vslib::RootComponent&   m_root;
        ::vslib::IConverter&      m_converter;
        ::vslib::ParameterSetting m_parameter_setting_task;
        ::vslib::ParameterMap     m_parameter_map;

        void onInitialization()
        {
            // everything generic that needs to be done to initialize the vloop
            m_init_done = true;
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
        }

        void onConfigured()
        {
            // initialize user code (RT)
            if (m_first)
            {
                m_converter.init();
                m_first = false;
            }

            // background task running continuously
            // other background tasks
            // ...
            //

            // user background task:
            m_converter.backgroundTask();

            // for testing purposes:
            // break;
        }

        TransResVS toConfiguring()
        {
            return m_parameter_setting_task.checkNewSettingsAvailable() ? TransResVS{VSStates::configuring}
                                                                        : TransResVS{};
        }

        TransResVS toInitialization()
        {
            // allow transition if all Parameters have been initialized
            return {VSStates::initialization};
        }

        TransResVS toUnconfiguredFromInit()
        {
            return (m_init_done) ? VSStates::unconfigured : VSStates::initialization;
        }

        TransResVS toUnconfigured()
        {
            const auto& parameter_registry = ParameterRegistry::instance();
            return parameter_registry.parametersInitialized() ? VSStates::configured : VSStates::unconfigured;
        }

        TransResVS toConfigured()
        {
            const auto& parameter_registry = ParameterRegistry::instance();
            return parameter_registry.parametersInitialized() ? VSStates::configured : VSStates::unconfigured;
        }
    };

}   // namespace vslib::utils
