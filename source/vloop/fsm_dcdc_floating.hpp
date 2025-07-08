//! @file
//! @brief Definition of finite state machine for floating DCDCs
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.hpp"
#include "pops_utils.hpp"

namespace user
{
    enum class DCDCFloatingVloopStates
    {
        FO,   // fault off
        FS,   // fault stopping
        OF,   // off
        SP,   // stopping
        ST,   // starting
        BK,   // blocking
        CH,   // charging
        CD,   // charged
        DT    // direct
    };

    class Converter;

    class DCDCFloatingStateMachine
    {
        using StateMachine = ::utils::Fsm<DCDCFloatingVloopStates, DCDCFloatingStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<DCDCFloatingVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<DCDCFloatingVloopStates> (DCDCFloatingStateMachine::*)();

      public:
        DCDCFloatingStateMachine(Converter&);

        void update();

        [[nodiscard]] DCDCFloatingVloopStates getState() const noexcept;

      private:
        StateMachine m_fsm;

        Converter& m_dcdc_floating;

        void onFaultOff();

        void onFaultStopping();

        void onOff();

        void onStopping();

        void onStarting();

        void onBlocking();

        void onCharging();

        void onCharged();

        void onDirect();

        TransRes toFaultOff();

        TransRes toFaultStopping();

        TransRes toOff();

        TransRes toStopping();

        TransRes toStarting();

        TransRes toBlocking();

        TransRes toCharging();

        TransRes toCharged();

        TransRes toDirect();
    };

}   // namespace user
