//! @file
//! @brief Definition of finite state machine for charger DCDCs
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.hpp"
#include "pops_utils.hpp"

namespace user
{
    enum class DCDCChargerVloopStates
    {
        FO,   // fault off
        FS,   // fault stopping
        OF,   // off
        SP,   // stopping
        ST,   // starting
        BK,   // blocking
        DT    // direct
    };

    class Converter;

    class DCDCChargerStateMachine
    {
        using StateMachine = ::utils::Fsm<DCDCChargerVloopStates, DCDCChargerStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<DCDCChargerVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<DCDCChargerVloopStates> (DCDCChargerStateMachine::*)();

      public:
        DCDCChargerStateMachine(Converter&);

        void update();

        [[nodiscard]] DCDCChargerVloopStates getState() const noexcept;

      private:
        StateMachine m_fsm;

        Converter& m_dcdc_charger;

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

        //! Transition to the SP state.
        TransRes toStopping();

        TransRes toStarting();

        TransRes toBlocking();

        TransRes toDirect();

        //! Checks if all Floating DCDC are in BK (BK) state.
        //!
        //! @return True if all Floating DCDCs are in BK, false otherwise
        bool checkAllFloatingVloopInBK();

        //! Returns the Vdc value of the connected Floating DCDC.
        //!
        //! @return Vdc value of the connected Floating DCDC, in V.
        double getVdcFloatings();
    };

}   // namespace user
