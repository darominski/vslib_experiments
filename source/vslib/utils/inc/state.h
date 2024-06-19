//! @file
//! @brief Definition of VSlib generic finite state machine
//! @author Dominik Arominski

#pragma once

#include <iostream>

#include "componentValidation.h"
#include "fsm2.h"
#include "parameterInitialized.h"
#include "parameterRegistry.h"

namespace vslib::utils
{
    enum class VSStates
    {
        off,
        idle,
        configured,
        unconfigured,
        fault
    };

    template<fgc4::utils::Enumeration UserStates>
    class VSMachine
    {
        using StateMachine = ::utils::Fsm<VSStates, UserStates, VSMachine, false>;

        using TransResVS = ::utils::FsmTransitionResult<std::variant<VSStates, UserStates>>;

        // using StateFunc = void (VSMachine::*)();
        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        // using TransitionFunc = ::utils::FsmTransitionResult<VSStates> (VSMachine::*)();
        using TransitionFunc = ::utils::FsmTransitionResult<UserStates> (*)();

      public:
        VSMachine()
            : m_fsm(VSStates::unconfigured)
        {
            // CAUTION: The order of transition method matters

            // clang-format off
            m_fsm.addState(VSStates::off,  &VSMachine::onOff,  {&VSMachine::toOff               });
            m_fsm.addState(VSStates::unconfigured,  &VSMachine::onUnconfigured,  {&VSMachine::toConfigured});
            m_fsm.addState(VSStates::configured,  &VSMachine::onConfigured,  {&VSMachine::toOff});

            // clang-format on
        }

        void
        addState(UserStates state, std::function<void(void)> onState, const std::vector<TransitionFunc>& transitions)
        {
            m_state_functions.insert({state, onState});
            // m_user_fsm.addState(state, &m_state_functions[state], transitions);
        }

        bool isConfigured() const
        {
            return m_fsm.getState() == VSStates::configured;
        }

        void update()
        {
            m_fsm.update();
        }

        [[nodiscard]] const auto& getState() const noexcept
        {
            return m_fsm.getState();
        }

      private:
        StateMachine m_fsm;
        // UserStateMachine m_user_fsm;

        std::map<UserStates, std::function<void(void)>> m_state_functions;

        void onOff()
        {
            // turn off the power converter
        }

        TransResVS toOff()
        {
            return {VSStates::idle};
        }

        void onUnconfigured()
        {
            // nothing to do?
        }

        void onConfigured()
        {
            // allow transitioning further
        }

        TransResVS toUnconfigured()
        {
            // transition logic
            return {VSStates::unconfigured};
        }

        TransResVS toConfigured()
        {
            if (!vslib::utils::parametersInitialized())
            {
                return {VSStates::unconfigured};
            }
            // check if all Parameters have been initialized
            return {VSStates::configured};
        }
    };

}   // namespace vslib::utils