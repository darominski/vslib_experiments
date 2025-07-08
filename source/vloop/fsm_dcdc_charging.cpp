#include "fsm_dcdc_charging.hpp"
#include "user.hpp"

namespace user
{
    DCDCChargerStateMachine::DCDCChargerStateMachine(Converter& dcdc_charger)
        : m_fsm(*this, DCDCChargerVloopStates::FO),
          m_dcdc_charger(dcdc_charger)
    {
        // Initialize handles for the i_loop state, vdc measurement, gateware status, interlock status, and the PFM
        // status

        // CAUTION: The order of transition method matters
        // clang-format off
            m_fsm.addState(DCDCChargerVloopStates::FO, &DCDCChargerStateMachine::onFaultOff,      {&DCDCChargerStateMachine::toOff});
            m_fsm.addState(DCDCChargerVloopStates::FS, &DCDCChargerStateMachine::onFaultStopping, {&DCDCChargerStateMachine::toFaultOff});
            m_fsm.addState(DCDCChargerVloopStates::OF, &DCDCChargerStateMachine::onOff,           {&DCDCChargerStateMachine::toFaultStopping, &DCDCChargerStateMachine::toStarting});
            m_fsm.addState(DCDCChargerVloopStates::SP, &DCDCChargerStateMachine::onStopping,      {&DCDCChargerStateMachine::toFaultStopping, &DCDCChargerStateMachine::toOff});
            m_fsm.addState(DCDCChargerVloopStates::ST, &DCDCChargerStateMachine::onStarting,      {&DCDCChargerStateMachine::toFaultStopping, &DCDCChargerStateMachine::toStopping, &DCDCChargerStateMachine::toBlocking});
            m_fsm.addState(DCDCChargerVloopStates::BK, &DCDCChargerStateMachine::onBlocking,      {&DCDCChargerStateMachine::toFaultStopping, &DCDCChargerStateMachine::toStopping, &DCDCChargerStateMachine::toDirect});
            m_fsm.addState(DCDCChargerVloopStates::DT, &DCDCChargerStateMachine::onDirect,        {&DCDCChargerStateMachine::toFaultStopping, &DCDCChargerStateMachine::toStopping, &DCDCChargerStateMachine::toBlocking});
        // clang-format on
    }

    void DCDCChargerStateMachine::update()
    {
        m_fsm.update();
    }

    [[nodiscard]] DCDCChargerVloopStates DCDCChargerStateMachine::getState() const noexcept
    {
        return m_fsm.getState();
    }

    void DCDCChargerStateMachine::onFaultOff()
    {
        // open the safety chain?
    }
    void DCDCChargerStateMachine::onFaultStopping()
    {
    }
    void DCDCChargerStateMachine::onOff()
    {
    }
    void DCDCChargerStateMachine::onStopping()
    {
    }
    void DCDCChargerStateMachine::onStarting()
    {
    }
    void DCDCChargerStateMachine::onBlocking()
    {
    }
    void DCDCChargerStateMachine::onCharging()
    {
    }
    void DCDCChargerStateMachine::onCharged()
    {
    }
    void DCDCChargerStateMachine::onDirect()
    {
    }

    DCDCChargerStateMachine::TransRes DCDCChargerStateMachine::toFaultOff()
    {
        if (m_dcdc_charger.getVdc() < constants::v_dc_min_threshold)   // DC bus is discharged
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::FO};
        }
        return {};
    }

    DCDCChargerStateMachine::TransRes DCDCChargerStateMachine::toFaultStopping()
    {
        if (checkGatewareFault() || checkInterlock() || m_dcdc_charger.getILoopState() == ILoopStates::FS
            || m_dcdc_charger.getPFMState() == PFMStates::FO)
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::FS};
        }
        return {};
    }

    DCDCChargerStateMachine::TransRes DCDCChargerStateMachine::toOff()
    {
        if (m_dcdc_charger.getILoopState() == ILoopStates::OF)
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::OF};
        }
        return {};
    }

    //! Transition to the SP state.
    DCDCChargerStateMachine::TransRes DCDCChargerStateMachine::toStopping()
    {
        if (m_dcdc_charger.getILoopState() == ILoopStates::SP || checkHMIRequestStop())
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::SP};
        }
        return {};
    }

    DCDCChargerStateMachine::TransRes DCDCChargerStateMachine::toStarting()
    {
        if (m_dcdc_charger.checkVSRunReceived())
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::ST};
        }
        return {};
    }

    DCDCChargerStateMachine::TransRes DCDCChargerStateMachine::toBlocking()
    {
        // from ST
        if (getState() == DCDCChargerVloopStates::ST && checkOutputsReady()
            && m_dcdc_charger.getVout() <= constants::v_out_threshold)
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::BK};
        }

        // from DT
        if (getState() == DCDCChargerVloopStates::DT && m_dcdc_charger.checkAllFloatingVloopInBK()
            && m_dcdc_charger.getVdcFloatings() < constants::v_dc_min_threshold)
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::BK};
        }
        return {};
    }

    DCDCChargerStateMachine::TransRes DCDCChargerStateMachine::toDirect()
    {
        if (m_dcdc_charger.checkUnblockReceived() && (m_dcdc_charger.getVloopMask() == 1))
        {
            return DCDCChargerStateMachine::TransRes{DCDCChargerVloopStates::DT};
        }
        return {};
    }

}   // namespace user
