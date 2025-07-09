//! @file
//! @brief Definition of finite state machine for the main controller
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "fsm_afe.hpp"
#include "fsm_crowbar.hpp"
#include "fsm_dcdc_charger.hpp"
#include "fsm_dcdc_floating.hpp"
#include "pops_constants.hpp"

namespace user
{
    enum class MCVloopStates
    {
        FO,   // fault OF
        FS,   // fault SP
        OF,   // off
        SP,   // stopping
        ST,   // starting
        BK,   // blocking
        CH,   // charging
        SB,   // standby
        CY    // cycling
    };

    class Converter;   // fwd declaration

    class MCStateMachine
    {
        using StateMachine = ::utils::Fsm<MCVloopStates, MCStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<MCVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<MCVloopStates> (MCStateMachine::*)();

      public:
        MCStateMachine(Converter&);

        void update();

        [[nodiscard]] MCVloopStates getState() const noexcept;

      private:
        StateMachine m_fsm;

        Converter& m_main;

        void onFaultOff();

        void onFaultStopping();

        void onOff();

        void onStopping();

        void onStarting();

        void onBlocking();

        void onCharging();

        void onStandby();

        void onCycling();

        TransRes toFaultOff();

        TransRes toFaultStopping();

        TransRes toOff();

        TransRes toStopping();

        TransRes toStarting();

        TransRes toBlocking();

        TransRes toCharging();

        TransRes toStandby();

        TransRes toCycling();

      private:
        //! Checks whether all connected AFEs' Vloops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if AFEs are in the expected state, false otherwise
        bool checkAFEStates(AFEStates state);

        //! Checks whether all connected Charger DCDCs' Vloop are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Charger DCDCs are in the expected state, false otherwise
        bool checkDCDCChargerVloopStates(DCDCChargerVloopStates state);

        //! Checks whether all connected Floating DCDCs' Vloops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Floating DCDCs are in the expected state, false otherwise
        bool checkDCDCFloatingVloopStates(DCDCFloatingStates state);

        //! Checks whether the Crowbar's i_loop is in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Crowbar is in the expected state, false otherwise
        bool checkCrowbarIloopState(IloopStates state);

        //! Checks whether the Charger DCDCs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Charger DCDCs are in the expected state, false otherwise
        bool checkDCDCChargerIloopStates(IloopStates state);

        //! Checks whether the Floating DCDCs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Floating DCDCs are in the expected state, false otherwise
        bool checkDCDCFloatingIloopStates(IloopStates state);

        //! Checks whether the AFEs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if AFEs are in the expected state, false otherwise
        bool checkAFEIloopStates(IloopStates state);

        //! Checks whether all connected AFEs' and DCDCs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if all connected AFEs and DCDCs are in the expected state, false otherwise
        bool checkAllIloops(IloopState state);

        bool checkIloopState(IloopState state);

        bool checkAllVloopsInOF();

        double getVdcFloatings();

        bool checkConsistentIloopVloop();
    };

}   // namespace user
