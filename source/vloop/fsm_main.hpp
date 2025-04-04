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

    class MCStateMachine
    {
        using StateMachine = ::utils::Fsm<MCVloopStates, MCStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<MCVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<MCVloopStates> (MCStateMachine::*)();

      public:
        MCStateMachine()
            : m_fsm(*this, MCVloopStates::FO)
        {
            // Initialize handles for the i_loop state, vdc measurement, gateware status, interlock status, and the PFM
            // status, and all i_loop and Vloops on executors

            // CAUTION: The order of transition method matters
            // clang-format off
            m_fsm.addState(MCVloopStates::FO, &MCStateMachine::onFaultOff,      {&MCStateMachine::toOff});
            m_fsm.addState(MCVloopStates::FS, &MCStateMachine::onFaultStopping, {&MCStateMachine::toFaultOff});
            m_fsm.addState(MCVloopStates::OF, &MCStateMachine::onOff,           {&MCStateMachine::toFaultStopping, &MCStateMachine::toStarting});
            m_fsm.addState(MCVloopStates::SP, &MCStateMachine::onStopping,      {&MCStateMachine::toFaultStopping, &MCStateMachine::toOff});
            m_fsm.addState(MCVloopStates::ST, &MCStateMachine::onStarting,      {&MCStateMachine::toFaultStopping, &MCStateMachine::toBlocking});
            m_fsm.addState(MCVloopStates::BK, &MCStateMachine::onBlocking,      {&MCStateMachine::toFaultStopping, &MCStateMachine::toStopping, &MCStateMachine::toCharging});
            m_fsm.addState(MCVloopStates::CH, &MCStateMachine::onCharging,      {&MCStateMachine::toFaultStopping, &MCStateMachine::toStopping, &MCStateMachine::toStandby});
            m_fsm.addState(MCVloopStates::SB, &MCStateMachine::onStandby,       {&MCStateMachine::toFaultStopping, &MCStateMachine::toStopping, &MCStateMachine::toBlocking, &MCStateMachine::toCycling});
            m_fsm.addState(MCVloopStates::CY, &MCStateMachine::onCycling,       {&MCStateMachine::toFaultStopping, &MCStateMachine::toStandby});
            // clang-format on
        }

        void update()
        {
            m_fsm.update();
        }

        [[nodiscard]] auto getState() const noexcept
        {
            return m_fsm.getState();
        }

      private:
        StateMachine m_fsm;

        void onFaultOff()
        {
            // open the safety chain?
        }
        void onFaultStopping()
        {
        }
        void onOff()
        {
        }
        void onStopping()
        {
        }
        void onStarting()
        {
        }
        void onBlocking()
        {
        }
        void onCharging()
        {
        }
        void onStandby()
        {
        }
        void onCycling()
        {
        }

        TransRes toFaultOff()
        {
            if (checkAllIloops(IloopState::FO))
            {
                return TransRes{MCVloopStates::FO};
            }

            // no transition
            return {};
        }

        TransRes toFaultStopping()
        {
            // from any state
            if (checkAllIloops(IloopState::FS) || checkGatewareFault() || checkInterlock()
                || i_loop.getState() == IloopStates::FS || pfm.getState() == PFMStates::FO || checkFaultChainOpen()
                || checkConsistentIloopVloops())
            {
                return TransRes{MCVloopStates::FS};
            }

            // from BK
            if (getState() == MCVloopStates::BK && i_loop.getState() == IloopStates::SP)
            {
                return TransRes{MCVloopStates::FS};
            }

            // no transition
            return {};
        }

        TransRes toOff()
        {
            if (i_loop.getState() == IloopStates::OF
                && ((getState() == MCVloopStates::SP && checkAllIloops(IloopState::OF))
                    || (getState() == MCVloopStates::FO && checkAllVloopsInOF() && checkCrowbarIloop(IloopStates::BK))))
            {
                return TransRes{MCVloopStates::OF};
            }

            // no transition
            return {};
        }

        //! Transition to the SP state.
        //!
        //! @param force_stop Force SP of the DCDC, e.g. from a HMI request
        TransRes toStopping()
        {
            // from BK
            if ((getState() == MCVloopStates::BK && checkAFEIloopStates(IloopStates::SP)
                 && checkDCDCChargerVloopStates(DCDCChargerVloopStates::SP)))
            {
                return TransRes{MCVloopStates::SP};
            }

            // from CH
            if (getState() == MCVloopStates::CH && checkDCDCChargerVloopStates(DCDCChargerVloopStates::SP)
                && checkAFEILoopStates(IloopStates::SP)
                && (checkDCDCFloatingIloopStates(IloopStates::SP) || checkDCDCFloatingIloopStates(IloopStates::BK)))
            {
                return TransRes{MCVloopStates::SP};
            }

            // from SB
            if (getState() == MCVloopStates::SB && checkHMIRequestStop() && checkAFEIloopStates(IloopStates::SP)
                && checkDCDCChargerVloopStates(DCDCChargerVloopStates::SP)
                && (checkDCDCFloatingIloopStates(IloopStates::SP) || checkDCDCFloatingIloopStates(IloopStates::BK)))
            {
                return TransRes{MCVloopStates::SP};
            }

            // no transition
            return {};
        }

        TransRes toStarting()
        {
            if (checkVSRunReceived())
            {
                return TransRes{MCVloopStates::ST};
            }

            // no transition
            return {};
        }

        TransRes toBlocking()
        {
            // from ST
            if (getState() == MCVloopStates::ST && checkAFEIloopStates(IloopStates::DT)
                && checkDCDCChargerIloopStates(IloopStates::BK))
            {
                return TransRes{MCVloopStates::BK};
            }

            // from  SB
            if (getState() == MCVloopStates::SB && checkDCDCFloatingIloopStates(IloopStates::BK)
                && checkDCDCChargerIloopStates(IloopStates::BK))
            {
                return TransRes{MCVloopStates::BK};
            }

            // no transition
            return {};
        }

        TransRes toCharging()
        {
            if (checkUnblockReceived())
            {
                return TransRes(MCVloopStates::CH);
            }

            // no transition
            return {};
        }

        TransRes toStandby()
        {
            // from CH
            if (getState() == MCVloopStates::CH && checkIloopState(IloopStates::SB)
                && checkAFEIloopStates(IloopStates::DT) && checkDCDCChargerVloopStates(DCDCChargerVloopStates::CH)
                && checkDCDCFloatingVloopStates(DCDCFloatingStates::CD))
            {
                return TransRes{MCVloopStates::SB};
            }

            // from CY
            if (getState() == MCVloopStates::CY && checkHMIRequestSB())
            {
                return TransRes{MCVloopStates::SB};
            }

            // no transition
            return {};
        }

        TransRes toCycling()
        {
            if (checkIloopState(IloopStates::CY) || checkIloopState(IloopStates::TC))
            {
                return TransRes{MCVloopStates::CY};
            }

            // no transition
            return {};
        }

      private:
        //! Checks whether all connected AFEs' Vloops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if AFEs are in the expected state, false otherwise
        bool checkAFEStates(AFEStates state)
        {
            // TODO: loop over all connected AFEs and check their state. If all are in the 'state', return true, false
            // otherwise.
            return false;
        }

        //! Checks whether all connected Charger DCDCs' Vloop are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Charger DCDCs are in the expected state, false otherwise
        bool checkDCDCChargerVloopStates(DCDCChargerVloopStates state)
        {
            // TODO: loop over all connected Charger DCDCs and check their state. If all are in the 'state', return
            // true, false otherwise.
            return false;
        }

        //! Checks whether all connected Floating DCDCs' Vloops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Floating DCDCs are in the expected state, false otherwise
        bool checkDCDCFloatingVloopStates(DCDCFloatingStates state)
        {
            // TODO: loop over all connected Floating DCDCs and check their state. If all are in the 'state', return
            // true, false otherwise.
            return false;
        }

        //! Checks whether the Crowbar's i_loop is in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Crowbar is in the expected state, false otherwise
        bool checkCrowbarIloopState(IloopStates state)
        {
            // TODO: check if the Crowbar's i_loop state is the same as the desired state
            return false;
        }

        //! Checks whether the Charger DCDCs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Charger DCDCs are in the expected state, false otherwise
        bool checkDCDCChargerIloopStates(IloopStates state)
        {
            // TODO: check if all Charger DCDC i_loop states are equal to the desired state
            return false;
        }

        //! Checks whether the Floating DCDCs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if Floating DCDCs are in the expected state, false otherwise
        bool checkDCDCFloatingIloopStates(IloopStates state)
        {
            // TODO: check if all Floating DCDC i_loop states are equal to the desired state
            return false;
        }

        //! Checks whether the AFEs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if AFEs are in the expected state, false otherwise
        bool checkAFEIloopStates(IloopStates state)
        {
            // TODO: check if all AFEs i_loop states are equal to the desired state
            return false;
        }

        //! Checks whether all connected AFEs' and DCDCs' i_loops are in the desired state.
        //!
        //! @param state Expected state
        //! @return True if all connected AFEs and DCDCs are in the expected state, false otherwise
        bool checkAllIloops(IloopState state)
        {
            // TODO: loop over all connected DCDCs, AFEs, and crowbar to check if their i_loop state is the same as the
            // desired one
            return (
                checkCrowbarIloopState(state) && checkDCDCChargerIloopStates(state)
                && checkDCDCFloatingIloopStates(state) && checkAFEIloopStates(state)
            );
        }

        bool checkIloopState(IloopState state)
        {
            return (i_loop.getState() == state);
        }

        bool checkAllVloopsInOF()
        {
            return (
                checkAFEVloopStates(AFEVloopStates::OF) && checkDCDCChargerVloopStates(DCDCChargerVloopStates::OF)
                && checkDCDCFloatingVloopStates(DCDCFloatingVloopStates::OF)
            );
        }

        double getVdcFloatings()
        {
            // TODO: get Vdc value of Floating DCDC
            return 0.0;
        }

        bool checkVSRunReceived()
        {
            // TODO: check if VS_RUN has been received from i_loop
            return false;
        }

        bool checkUnblockReceived()
        {
            // TODO: check if 'Unblock' has been received from i_loop
            return false;
        }

        bool checkOutputsReady()
        {
            // TODO: check if outputs == 0110
            return false;
        }

        bool checkFaultChainOpen()
        {
            // TODO: check if the fault chain has been opened
            return false;
        }

        bool checkConsistentIloopVloop()
        {
            // TODO: check consistency of the set of i_loops and Vloops with HMI, e.g. if AFEs and DCDCs in SP and no
            // HMI command to SP
            return false;
        }

        bool getVloopMask()
        {
            // TODO: get Vloop mask setting
            return false;
        }

        double getVout()
        {
            // TODO: get V out
            return 0.0;
        }

        double getVdc()
        {
            // TODO: get V dc
            return 0.0;
        }
    };


}   // namespace user
