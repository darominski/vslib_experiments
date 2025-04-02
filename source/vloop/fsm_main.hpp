//! @file
//! @brief Definition of finite state machine for the main controller
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "fsm_afe.hpp"
#include "fsm_crowbar.hpp"
#include "fsm_dcdc_charging.hpp"
#include "fsm_dcdc_floating.hpp"
#include "pops_constants.h"

namespace user
{
    enum class MainStates
    {
        fault_off,
        fault_stopping,
        off,
        stopping,
        starting,
        blocking,
        charging,
        standby,
        cycling
    };

    class MainFSM
    {
        using StateMachine = ::utils::Fsm<MainStates, MainFSM, false>;

        using TransRes = ::utils::FsmTransitionResult<MainStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<MainStates> (MainFSM::*)();

      public:
        MainFSM()
            : m_fsm(*this, MainStates::fault_off)
        {
            // Initialize handles for the I_loop state, vdc measurement, gateware status, interlock status, and the PFM
            // status, and all I_loop and Vloops on executors

            // CAUTION: The order of transition method matters
            // clang-format off
            m_fsm.addState(MainStates::fault_off,      &MainFSM::onFaultOff,      {&MainFSM::toOff});
            m_fsm.addState(MainStates::fault_stopping, &MainFSM::onFaultStopping, {&MainFSM::toFaultOff});
            m_fsm.addState(MainStates::off,            &MainFSM::onOff,           {&MainFSM::toFaultStopping, &MainFSM::toStarting});
            m_fsm.addState(MainStates::stopping,       &MainFSM::onStopping,      {&MainFSM::toFaultStopping, &MainFSM::toOff});
            m_fsm.addState(MainStates::starting,       &MainFSM::onStarting,      {&MainFSM::toFaultStopping, &MainFSM::toBlocking});
            m_fsm.addState(MainStates::blocking,       &MainFSM::onBlocking,      {&MainFSM::toFaultStopping, &MainFSM::toStopping, &MainFSM::toCharging});
            m_fsm.addState(MainStates::charging,       &MainFSM::onCharging,      {&MainFSM::toFaultStopping, &MainFSM::toStopping, &MainFSM::toStandby});
            m_fsm.addState(MainStates::standby,        &MainFSM::onStandby,       {&MainFSM::toFaultStopping, &MainFSM::toStopping, &MainFSM::toBlocking, &MainFSM::toCycling});
            m_fsm.addState(MainStates::cycling,        &MainFSM::onCycling,       {&MainFSM::toFaultStopping, &MainFSM::toStandby});
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
            if (checkAllIloops(RegLoopState::FO))
            {
                return TransRes{MainStates::fault_off};
            }
            return {};
        }

        TransRes toFaultStopping(const bool force_stop = false)
        {
            if (force_stop || checkAllIloops(RegLoopState::FS) || checkGatewareFault() || checkInterlock()
                || I_loop.getState() == RegLoopStates::FS || pfm.getState() == PFMStates::FO || checkFaultChainOpen()
                || checkConsistentIloopVloops()
                || (getState() == MainStates::blocking && I_loop.getState() == RegLoopStates::SP))
            {
                return TransRes{MainStates::fault_stopping};
            }
            return {};
        }

        TransRes toOff()
        {
            if (I_loop.getState() == RegLoopStates::OF
                && ((getState() == MainStates::SP && checkAllIloops(RegLoopState::OF))
                    || (getState() == MainStates::FO && checkAllVloopsInOF() && checkCrowbarIloop(RegLoopStates::BK))))
            {
                return TransRes{MainStates::off};
            }
            return {};
        }

        //! Transition to the stopping state.
        //!
        //! @param force_stop Force stopping of the DCDC, e.g. from a HMI request
        TransRes toStopping(const bool force_stop = false)
        {
            if ((getState() == MainStates::blocking && checkAFEStates(AFEStates::stopping)
                 && checkDCDCChargerStates(DCDCChargerStates::stopping))
                || (getState() == MainStates::charging && force_stop
                    && checkDCDCChargerStates(DCDCChargerStates::stopping) && checkAFEILoopStates(RegLoopStates::SP)
                    && (checkDCDCFloatingIloopStates(RegLoopStates::SP)
                        || checkDCDCFloatingIloopStates(RegLoopStates::BK))))
            {
                return TransRes{MainStates::stopping};
            }
            return {};
        }

        TransRes toStarting()
        {
            if (checkVSRunReceived())
            {
                return TransRes{MainStates::starting};
            }
            return {};
        }

        TransRes toBlocking()
        {
            if ((getState() == MainStates::starting && checkAFEIloopStates(RegLoopStates::DT)
                 && checkDCDCChargerIloopStates(RegLoopStates::BK))
                || (getState() == MainStates::standby && checkDCDCFloatingIloopStates(RegLoopStates::BK)
                    && checkDCDCChargerIloopStates(RegLoopStates::BK)))
            {
                return TransRes{MainStates::blocking};
            }
            return {};
        }

        TransRes toCharging()
        {
            if (checkUnblockReceived())
            {
                return TransRes(MainStates::charging);
            }
            return {};
        }

        //! Transition function to standby state.
        //!
        //! @param force_standby Forces transition to standby from cycling, e.g. by HMI request
        TransRes toStandby()
        {
            if ((getState() == MainStates::charging && checkRegLoopState(RegLoopStates::SB)
                 && checkAFEIloopStates(RegLoopStates::DT) && checkDCDCChargerStates(DCDCChargerStates::charging)
                 && checkDCDCFloatingStates(DCDCFloatingStates::charged))
                || (getState() == MainStates::cycling && checkHMIRequestSB()))
            {
                return TransRes{MainStates::standby};
            }
            return {};
        }

        TransRes toCycling()
        {
            if (checkRegLoopState(RegLoopStates::CY) || checkRegLoopState(RegLoopStates::TC))
            {
                return TransRes{MainStates::cycling};
            }
            return {};
        }

      private:
        //! Checks whether all connected AFEs are in the desired state.
        //!
        //! @param state Expected state of the connected AFEs
        //! @return True if AFEs are in the expected state, false otherwise
        bool checkAFEStates(AFEStates state)
        {
            // TODO: loop over all connected AFEs and check their state. If all are in the 'state', return true, false
            // otherwise.
            return false;
        }

        //! Checks whether all connected Charger DCDCs are in the desired state.
        //!
        //! @param state Expected state of the connected Charger DCDCs
        //! @return True if Charger DCDCs are in the expected state, false otherwise
        bool checkDCDCChargerStates(DCDCChargerStates state)
        {
            // TODO: loop over all connected Charger DCDCs and check their state. If all are in the 'state', return
            // true, false otherwise.
            return false;
        }

        //! Checks whether all connected Floating DCDCs are in the desired state.
        //!
        //! @param state Expected state of the connected Floating DCDCs
        //! @return True if Floating DCDCs are in the expected state, false otherwise
        bool checkDCDCFloatingStates(DCDCFloatingStates state)
        {
            // TODO: loop over all connected Floating DCDCs and check their state. If all are in the 'state', return
            // true, false otherwise.
            return false;
        }

        //! Checks whether the Crowbar is in the desired state.
        //!
        //! @param state Expected state of the Crowbar
        //! @return True if Crowbar are in the expected state, false otherwise
        bool checkCrowbarStates(CrowbarStates state)
        {
            // TODO: check crowbar state. If it is in the 'state', return true, false otherwise.
            return false;
        }

        //!
        bool checkCrowbarIloopState(RegLoopStates state)
        {
            // TODO: check if the Crowbar's I_loop state is the same as the desired state
            return false;
        }

        bool checkDCDCChargerIloopStates(RegLoopStates state)
        {
            // TODO: check if all Charger DCDC I_loop states are equal to the desired state
            return false;
        }

        bool checkDCDCFloatingIloopStates(RegLoopStates state)
        {
            // TODO: check if all Floating DCDC I_loop states are equal to the desired state
            return false;
        }

        bool checkAFEIloopStates(RegLoopStates state)
        {
            // TODO: check if all AFEs I_loop states are equal to the desired state
            return false;
        }

        bool checkAllIloops(RegLoopState state)
        {
            // TODO: loop over all connected DCDCs, AFEs, and crowbar to check if their I_loop state is the same as the
            // desired one
            return (
                checkCrowbarIloopState(state) && checkDCDCChargerIloopStates(state)
                && checkDCDCFloatingIloopStates(state) && checkAFEIloopStates(state)
            );
        }

        bool checkRegLoopState(RegLoopState state)
        {
            return (I_loop.getState() == state);
        }

        bool checkAllVloopsInOF()
        {
            return (
                checkAFEStates(AFEStates::off) && checkDCDCChargerStates(DCDCChargerStates::off)
                && checkDCDCFloatingStates(DCDCFloatingStates::off) && checkCrowbarIloop(RegLoopStates::BK)
            );
        }

        double getVdcFloatings()
        {
            // TODO: get Vdc value of Floating DCDC
            return 0.0;
        }

        bool checkVSRunReceived()
        {
            // TODO: check if VS_RUN has been received from I_loop
            return false;
        }

        bool checkUnblockReceived()
        {
            // TODO: check if 'Unblock' has been received from I_loop
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
            // TODO: check consistency of the set of I_loops and Vloops with HMI, e.g. if AFEs and DCDCs in SP and no
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
