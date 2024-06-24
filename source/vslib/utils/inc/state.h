//! @file
//! @brief Definition of VSlib generic finite state machine
//! @author Dominik Arominski

#pragma once

#include <iostream>

#include "componentValidation.h"
#include "fsm.h"
#include "parameterInitialized.h"
#include "parameterRegistry.h"

namespace vslib::utils
{
    enum class VSStates
    {
        initialization,
        configured,
        unconfigured,
        user,
        fault
    };

    class VSMachine
    {
        using StateMachine = ::utils::Fsm<VSStates, VSMachine, false>;

        using TransResVS = ::utils::FsmTransitionResult<VSStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<VSStates> (VSMachine::*)();

      public:
        VSMachine()
            : m_fsm(*this, VSStates::unconfigured)
        {
            // CAUTION: The order of transition method matters

            // clang-format off
            m_fsm.addState(VSStates::initialization,  &VSMachine::onInitialization,  {&VSMachine::toUnconfigured});
            m_fsm.addState(VSStates::unconfigured,    &VSMachine::onUnconfigured,  {&VSMachine::toConfigured});
            m_fsm.addState(VSStates::configured,      &VSMachine::onConfigured,  {&VSMachine::toUser});
            // clang-format on
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

        void onInitialization()
        {
            // everything generic that needs to be done to initialize the vloop
        }

        void onUnconfigured()
        {
            // call user-defined method: converter.init()
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
            // allow transition if all Parameters have been initialized
            if (vslib::utils::parametersInitialized())
            {
                return {VSStates::configured};
            }
            // remain in the unconfigured state otherwise
            return {VSStates::unconfigured};
        }

        TransResVS toUser()
        {
            // everything is configured, allow moving to the user-defined FSM
            return {VSStates::user};
        }
    };

}   // namespace vslib::utils