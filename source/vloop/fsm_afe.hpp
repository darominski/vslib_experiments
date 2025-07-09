//! @file
//! @brief Definition of finite state machine for Active Front-Ends
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.hpp"
#include "pops_utils.hpp"

namespace user
{
    enum class AFEVloopStates
    {
        FO,   // fault_off
        FS,   // fault stopping
        OF,   // off
        SP,   // stopping
        PH,   // precharging
        PD,   // precharged
        ON    // on
    };

    class Converter;

    class AFEStateMachine
    {
        using StateMachine = ::utils::Fsm<AFEVloopStates, AFEStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<AFEVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<AFEVloopStates> (AFEStateMachine::*)();

      public:
        AFEStateMachine(Converter&);

        void update();

        [[nodiscard]] AFEVloopStates getState() const noexcept;

      private:
        StateMachine m_fsm;

        Converter& m_afe;

        void onFaultOff();

        void onFaultStopping();

        void onOff();

        void onStopping();

        void onPrecharging();

        void onPrecharged();

        void onOn();

        TransRes toFaultOff();

        TransRes toFaultStopping();

        TransRes toOff();

        //! Transition to the stopping state.
        //!
        //! @param force_stop Force stopping of the DCDC, e.g. from a HMI request
        TransRes toStopping();

        TransRes toPrecharging();

        TransRes toPrecharged();

        TransRes toOn();
    };
}   // namespace user
