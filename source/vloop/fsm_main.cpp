#include "fsm_main.hpp"
#include "user.hpp"

namespace user
{
    MCStateMachine(Converter& main)
        : m_fsm(*this, MCVloopStates::FO),
          m_main(main)
    {
        // Initialize handles for the m_main state, vdc measurement, gateware status, interlock status, and the PFM
        // status, and all m_main and Vloops on executors

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

    void MCStateMachine::update()
    {
        m_fsm.update();
    }

    [[nodiscard]] MCVloopStates MCStateMachine::getState() const noexcept
    {
        return m_fsm.getState();
    }

    void MCStateMachine::onFaultOff()
    {
        // open the safety chain?
    }
    void MCStateMachine::onFaultStopping()
    {
    }
    void MCStateMachine::onOff()
    {
    }
    void MCStateMachine::onStopping()
    {
    }
    void MCStateMachine::onStarting()
    {
    }
    void MCStateMachine::onBlocking()
    {
    }
    void MCStateMachine::onCharging()
    {
    }
    void MCStateMachine::onStandby()
    {
    }
    void MCStateMachine::onCycling()
    {
    }

    MCStateMachine::TransRes MCStateMachine::toFaultOff()
    {
        if (checkAllIloops(ILoopState::FO))
        {
            return TransRes{MCVloopStates::FO};
        }

        // no transition
        return {};
    }

    MCStateMachine::TransRes MCStateMachine::toFaultStopping()
    {
        // from any state
        if (checkAllIloops(ILoopState::FS) || checkGatewareFault() || checkInterlock()
            || m_main.getState() == ILoopStates::FS || pfm.getState() == PFMStates::FO || checkFaultChainOpen()
            || checkConsistentIloopVloops())
        {
            return TransRes{MCVloopStates::FS};
        }

        // from BK
        if (getState() == MCVloopStates::BK && m_main.getState() == ILoopStates::SP)
        {
            return TransRes{MCVloopStates::FS};
        }

        // no transition
        return {};
    }

    MCStateMachine::TransRes MCStateMachine::toOff()
    {
        if (m_main.getState() == ILoopStates::OF
            && ((getState() == MCVloopStates::SP && checkAllIloops(ILoopState::OF))
                || (getState() == MCVloopStates::FO && checkAllVloopsInOF() && checkCrowbarIloop(ILoopStates::BK))))
        {
            return TransRes{MCVloopStates::OF};
        }

        // no transition
        return {};
    }

    //! Transition to the SP state.
    //!
    //! @param force_stop Force SP of the DCDC, e.g. from a HMI request
    MCStateMachine::TransRes MCStateMachine::toStopping()
    {
        // from BK
        if ((getState() == MCVloopStates::BK && checkAFEILoopStates(ILoopStates::SP)
             && checkDCDCChargerVloopStates(DCDCChargerVloopStates::SP)))
        {
            return TransRes{MCVloopStates::SP};
        }

        // from CH
        if (getState() == MCVloopStates::CH && checkDCDCChargerVloopStates(DCDCChargerVloopStates::SP)
            && checkAFEILoopStates(ILoopStates::SP)
            && (checkDCDCFloatingILoopStates(ILoopStates::SP) || checkDCDCFloatingILoopStates(ILoopStates::BK)))
        {
            return TransRes{MCVloopStates::SP};
        }

        // from SB
        if (getState() == MCVloopStates::SB && checkHMIRequestStop() && checkAFEILoopStates(ILoopStates::SP)
            && checkDCDCChargerVloopStates(DCDCChargerVloopStates::SP)
            && (checkDCDCFloatingILoopStates(ILoopStates::SP) || checkDCDCFloatingILoopStates(ILoopStates::BK)))
        {
            return TransRes{MCVloopStates::SP};
        }

        // no transition
        return {};
    }

    MCStateMachine::TransRes MCStateMachine::toStarting()
    {
        if (checkVSRunReceived())
        {
            return TransRes{MCVloopStates::ST};
        }

        // no transition
        return {};
    }

    MCStateMachine::TransRes MCStateMachine::toBlocking()
    {
        // from ST
        if (getState() == MCVloopStates::ST && checkAFEILoopStates(ILoopStates::DT)
            && checkDCDCChargerILoopStates(ILoopStates::BK))
        {
            return TransRes{MCVloopStates::BK};
        }

        // from  SB
        if (getState() == MCVloopStates::SB && checkDCDCFloatingILoopStates(ILoopStates::BK)
            && checkDCDCChargerILoopStates(ILoopStates::BK))
        {
            return TransRes{MCVloopStates::BK};
        }

        // no transition
        return {};
    }

    MCStateMachine::TransRes MCStateMachine::toCharging()
    {
        if (checkUnblockReceived())
        {
            return TransRes(MCVloopStates::CH);
        }

        // no transition
        return {};
    }

    MCStateMachine::TransRes MCStateMachine::toStandby()
    {
        // from CH
        if (getState() == MCVloopStates::CH && checkILoopState(ILoopStates::SB) && checkAFEILoopStates(ILoopStates::DT)
            && checkDCDCChargerVloopStates(DCDCChargerVloopStates::CH)
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

    MCStateMachine::TransRes MCStateMachine::toCycling()
    {
        if (checkILoopState(ILoopStates::CY) || checkILoopState(ILoopStates::TC))
        {
            return TransRes{MCVloopStates::CY};
        }

        // no transition
        return {};
    }

    //! Checks whether all connected AFEs' Vloops are in the desired state.
    //!
    //! @param state Expected state
    //! @return True if AFEs are in the expected state, false otherwise
    bool MCStateMachine::checkAFEStates(AFEStates state)
    {
        // TODO: loop over all connected AFEs and check their state. If all are in the 'state', return true, false
        // otherwise.
        return false;
    }

    //! Checks whether all connected Charger DCDCs' Vloop are in the desired state.
    //!
    //! @param state Expected state
    //! @return True if Charger DCDCs are in the expected state, false otherwise
    bool MCStateMachine::checkDCDCChargerVloopStates(DCDCChargerVloopStates state)
    {
        // TODO: loop over all connected Charger DCDCs and check their state. If all are in the 'state', return
        // true, false otherwise.
        return false;
    }

    //! Checks whether all connected Floating DCDCs' Vloops are in the desired state.
    //!
    //! @param state Expected state
    //! @return True if Floating DCDCs are in the expected state, false otherwise
    bool MCStateMachine::checkDCDCFloatingVloopStates(DCDCFloatingStates state)
    {
        // TODO: loop over all connected Floating DCDCs and check their state. If all are in the 'state', return
        // true, false otherwise.
        return false;
    }

    //! Checks whether the Crowbar's m_main is in the desired state.
    //!
    //! @param state Expected state
    //! @return True if Crowbar is in the expected state, false otherwise
    bool MCStateMachine::checkCrowbarILoopState(ILoopStates state)
    {
        // TODO: check if the Crowbar's m_main state is the same as the desired state
        return false;
    }

    //! Checks whether the Charger DCDCs' m_mains are in the desired state.
    //!
    //! @param state Expected state
    //! @return True if Charger DCDCs are in the expected state, false otherwise
    bool MCStateMachine::checkDCDCChargerILoopStates(ILoopStates state)
    {
        // TODO: check if all Charger DCDC m_main states are equal to the desired state
        return false;
    }

    //! Checks whether the Floating DCDCs' m_mains are in the desired state.
    //!
    //! @param state Expected state
    //! @return True if Floating DCDCs are in the expected state, false otherwise
    bool MCStateMachine::checkDCDCFloatingILoopStates(ILoopStates state)
    {
        // TODO: check if all Floating DCDC m_main states are equal to the desired state
        return false;
    }

    //! Checks whether the AFEs' m_mains are in the desired state.
    //!
    //! @param state Expected state
    //! @return True if AFEs are in the expected state, false otherwise
    bool MCStateMachine::checkAFEILoopStates(ILoopStates state)
    {
        // TODO: check if all AFEs m_main states are equal to the desired state
        return false;
    }

    //! Checks whether all connected AFEs' and DCDCs' m_mains are in the desired state.
    //!
    //! @param state Expected state
    //! @return True if all connected AFEs and DCDCs are in the expected state, false otherwise
    bool MCStateMachine::checkAllIloops(ILoopState state)
    {
        // TODO: loop over all connected DCDCs, AFEs, and crowbar to check if their m_main state is the same as the
        // desired one
        return (
            checkCrowbarILoopState(state) && checkDCDCChargerILoopStates(state) && checkDCDCFloatingILoopStates(state)
            && checkAFEILoopStates(state)
        );
    }

    bool MCStateMachine::checkILoopState(ILoopState state)
    {
        return (m_main.getState() == state);
    }

    bool MCStateMachine::checkAllVloopsInOF()
    {
        return (
            checkAFEVloopStates(AFEVloopStates::OF) && checkDCDCChargerVloopStates(DCDCChargerVloopStates::OF)
            && checkDCDCFloatingVloopStates(DCDCFloatingVloopStates::OF)
        );
    }

    double MCStateMachine::getVdcFloatings()
    {
        // TODO: get Vdc value of Floating DCDC
        return 0.0;
    }

    bool MCStateMachine::checkVSRunReceived()
    {
        // TODO: check if VS_RUN has been received from m_main
        return false;
    }

    bool MCStateMachine::checkUnblockReceived()
    {
        // TODO: check if 'Unblock' has been received from m_main
        return false;
    }

    bool MCStateMachine::checkOutputsReady()
    {
        // TODO: check if outputs == 0110
        return false;
    }

    bool MCStateMachine::checkFaultChainOpen()
    {
        // TODO: check if the fault chain has been opened
        return false;
    }

    bool MCStateMachine::checkConsistentIloopVloop()
    {
        // TODO: check consistency of the set of m_mains and Vloops with HMI, e.g. if AFEs and DCDCs in SP and no
        // HMI command to SP
        return false;
    };


}   // namespace user
