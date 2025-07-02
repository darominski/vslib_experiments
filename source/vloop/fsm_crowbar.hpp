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

    class Converter;

    class CWBStateMachine
    {
        using StateMachine = ::utils::Fsm<CWBVloopStates, CWBStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<CWBVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<CWBVloopStates> (CWBStateMachine::*)();

      public:
        CWBStateMachine(Converter& crowbar);

        void update();

        [[nodiscard]] CWBVloopStates getState() const noexcept;

      private:
        StateMachine m_fsm;

        Converter& m_crowbar;

        void onFaultOff();

        void onOn();

        TransRes toOn();

        TransRes toFaultOff();
    };

}   // namespace user
