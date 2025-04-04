//! @file
//! @brief Definition of Finite state machine for a crowbar
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.hpp"
#include "pops_utils.hpp"

namespace user
{
    enum class CWBVloopStates
    {
        FO,   // fault off
        ON    // on
    };

    class CWBStateMachine
    {
        using StateMachine = ::utils::Fsm<CWBVloopStates, CWBStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<CWBVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<CWBVloopStates> (CWBStateMachine::*)();

      public:
        CWBStateMachine()
            : m_fsm(*this, CWBVloopStates::FO)
        {
            // obtain handles for the i_loop state and the intertrip light state

            // CAUTION: The order of transition method matters
            // clang-format off
            m_fsm.addState(CWBVloopStates::FO, &CWBStateMachine::onFaultOff, {&CWBStateMachine::toOn});
            m_fsm.addState(CWBVloopStates::ON, &CWBStateMachine::onOn,       {&CWBStateMachine::toFaultOff});
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

        void onFaultOff()
        {
            // open the safety chain?
        }

        void onOn()
        {
        }

        TransRes toOn()
        {
            if (checkVSRunReceived())
            {
                return TransRes{CWBVloopStates::ON};
            }
            return {};
        }

        TransRes toFaultOff()
        {
            if (!checkIntertripLight() || i_loop.getState() == IloopStates::FO)
            {
                return TransRes{CWBVloopStates::FO};
            }
            return {};
        }
    };

}   // namespace user
