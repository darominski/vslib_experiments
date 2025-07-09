#include "fsm_afe.hpp"
#include "user.hpp"

namespace user
{
    AFEStateMachine::AFEStateMachine(Converter& afe)
        : m_fsm(*this, AFEVloopStates::FO),
          m_afe(afe)
    {
        // Initialize handles for the m_afe state, vdc measurement, gateware status, interlock status, and the PFM
        // status

        // CAUTION: The order of transition method matters
        // clang-format off
            m_fsm.addState(AFEVloopStates::FO, &AFEStateMachine::onFaultOff,      {&AFEStateMachine::toOff});
            m_fsm.addState(AFEVloopStates::FS, &AFEStateMachine::onFaultStopping, {&AFEStateMachine::toFaultOff});
            m_fsm.addState(AFEVloopStates::OF, &AFEStateMachine::onOff,           {&AFEStateMachine::toFaultStopping, &AFEStateMachine::toPrecharging});
            m_fsm.addState(AFEVloopStates::SP, &AFEStateMachine::onStopping,      {&AFEStateMachine::toFaultStopping, &AFEStateMachine::toOff});
            m_fsm.addState(AFEVloopStates::PH, &AFEStateMachine::onPrecharging,   {&AFEStateMachine::toFaultStopping, &AFEStateMachine::toPrecharged});
            m_fsm.addState(AFEVloopStates::PD, &AFEStateMachine::onPrecharged,    {&AFEStateMachine::toFaultStopping, &AFEStateMachine::toOn});
            m_fsm.addState(AFEVloopStates::ON, &AFEStateMachine::onOn,            {&AFEStateMachine::toFaultStopping, &AFEStateMachine::toStopping});
        // clang-format on
    }

    void AFEStateMachine::update()
    {
        m_fsm.update();
    }

    [[nodiscard]] AFEVloopStates AFEStateMachine::getState() const noexcept
    {
        return m_fsm.getState();
    }

    void AFEStateMachine::onFaultOff()
    {
        // open the safety chain?
    }
    void AFEStateMachine::onFaultStopping()
    {
    }
    void AFEStateMachine::onOff()
    {
    }
    void AFEStateMachine::onStopping()
    {
    }
    void AFEStateMachine::onPrecharging()
    {
    }
    void AFEStateMachine::onPrecharged()
    {
    }
    void AFEStateMachine::onOn()
    {
    }

    AFEStateMachine::AFEStateMachine::TransRes AFEStateMachine::toFaultOff()
    {
        // DC bus is discharged, MCB should be open, and MV and LV breakers should be open
        if (m_afe.getVdc() < constants::v_dc_min_threshold && m_afe.checkCBOpen() && m_afe.checkK1Open()
            && m_afe.checkK2Open())
        {
            return AFEStateMachine::TransRes{AFEVloopStates::FO};
        }
        return {};
    }

    AFEStateMachine::AFEStateMachine::TransRes AFEStateMachine::toFaultStopping()
    {
        if (checkGatewareFault() || checkInterlock() || m_afe.getILoopState() == ILoopStates::FS
            || m_afe.getPFMState() == PFMStates::FO   // TODO: this will be an independent PFM
            || (getState() == AFEVloopStates::PD && m_afe.getILoopState() == ILoopStates::SP))
        {
            return AFEStateMachine::TransRes{AFEVloopStates::FS};
        }
        return {};
    }

    AFEStateMachine::TransRes AFEStateMachine::toOff()
    {
        if (m_afe.getILoopState() == ILoopStates::OF)
        {
            return AFEStateMachine::TransRes{AFEVloopStates::OF};
        }
        return {};
    }

    //! Transition to the stopping state.
    //!
    //! @param force_stop Force stopping of the DCDC, e.g. from a HMI request
    AFEStateMachine::TransRes AFEStateMachine::toStopping()
    {
        if (m_afe.getILoopState() == ILoopStates::SP)
        {
            return AFEStateMachine::TransRes{AFEVloopStates::SP};
        }
        return {};
    }

    AFEStateMachine::TransRes AFEStateMachine::toPrecharging()
    {
        if (m_afe.checkVSRunReceived())
        {
            return AFEStateMachine::TransRes{AFEVloopStates::PH};
        }
        return {};
    }

    AFEStateMachine::TransRes AFEStateMachine::toPrecharged()
    {
        if (!m_afe.checkCBOpen() && m_afe.checkK1Open() && m_afe.checkK3Open()
            && m_afe.getVdc() >= constants::v_dc_charger_min)
        {
            return AFEStateMachine::TransRes{AFEVloopStates::PD};
        }
        return {};
    }

    AFEStateMachine::TransRes AFEStateMachine::toOn()
    {
        if (m_afe.checkUnblockReceived())
        {
            return AFEStateMachine::TransRes{AFEVloopStates::ON};
        }
        return {};
    }

}   // namespace user
