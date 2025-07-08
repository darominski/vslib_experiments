#include "fsm_dcdc_floating.hpp"
#include "pops_utils.hpp"
#include "user.hpp"

namespace user
{
    DCDCFloatingStateMachine::DCDCFloatingStateMachine(Converter& dcdc_floating)
        : m_fsm(*this, DCDCFloatingVloopStates::FO),
          m_dcdc_floating(dcdc_floating)
    {
        // Initialize handles for the i_loop state, gateware status, interlock status, and PFM state

        // CAUTION: The order of transition method matters
        // clang-format off
            m_fsm.addState(DCDCFloatingVloopStates::FO, &DCDCFloatingStateMachine::onFaultOff,      {&DCDCFloatingStateMachine::toOff});
            m_fsm.addState(DCDCFloatingVloopStates::FS, &DCDCFloatingStateMachine::onFaultStopping, {&DCDCFloatingStateMachine::toFaultOff});
            m_fsm.addState(DCDCFloatingVloopStates::OF, &DCDCFloatingStateMachine::onOff,           {&DCDCFloatingStateMachine::toFaultStopping, &DCDCFloatingStateMachine::toStarting});
            m_fsm.addState(DCDCFloatingVloopStates::SP, &DCDCFloatingStateMachine::onStopping,      {&DCDCFloatingStateMachine::toFaultStopping, &DCDCFloatingStateMachine::toOff});
            m_fsm.addState(DCDCFloatingVloopStates::ST, &DCDCFloatingStateMachine::onStarting,      {&DCDCFloatingStateMachine::toFaultStopping, &DCDCFloatingStateMachine::toStopping, &DCDCFloatingStateMachine::toBlocking});
            m_fsm.addState(DCDCFloatingVloopStates::BK, &DCDCFloatingStateMachine::onBlocking,      {&DCDCFloatingStateMachine::toFaultStopping, &DCDCFloatingStateMachine::toStopping, &DCDCFloatingStateMachine::toCharging});
            m_fsm.addState(DCDCFloatingVloopStates::CH, &DCDCFloatingStateMachine::onCharging,      {&DCDCFloatingStateMachine::toFaultStopping, &DCDCFloatingStateMachine::toStopping, &DCDCFloatingStateMachine::toCharged});
            m_fsm.addState(DCDCFloatingVloopStates::CD, &DCDCFloatingStateMachine::onCharged,       {&DCDCFloatingStateMachine::toFaultStopping, &DCDCFloatingStateMachine::toStopping, &DCDCFloatingStateMachine::toDirect});
            m_fsm.addState(DCDCFloatingVloopStates::DT, &DCDCFloatingStateMachine::onDirect,        {&DCDCFloatingStateMachine::toFaultStopping, &DCDCFloatingStateMachine::toStopping, &DCDCFloatingStateMachine::toCharged});
        // clang-format on
    }

    void DCDCFloatingStateMachine::update()
    {
        m_fsm.update();
    }

    [[nodiscard]] DCDCFloatingVloopStates DCDCFloatingStateMachine::getState() const noexcept
    {
        return m_fsm.getState();
    }

    void DCDCFloatingStateMachine::onFaultOff()
    {
        // open the safety chain?
    }
    void DCDCFloatingStateMachine::onFaultStopping()
    {
    }
    void DCDCFloatingStateMachine::onOff()
    {
    }
    void DCDCFloatingStateMachine::onStopping()
    {
    }
    void DCDCFloatingStateMachine::onStarting()
    {
    }
    void DCDCFloatingStateMachine::onBlocking()
    {
        // send VS power ON
        // send VS ready
    }
    void DCDCFloatingStateMachine::onCharging()
    {
    }
    void DCDCFloatingStateMachine::onCharged()
    {
    }
    void DCDCFloatingStateMachine::onDirect()
    {
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toFaultOff()
    {
        // from FS and DC bus is discharged
        if (m_dcdc_floating.getVdc() < constants::v_dc_min_threshold)
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::FO};
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toFaultStopping()
    {
        if (checkGatewareFault() || checkInterlock() || m_dcdc_floating.getILoopState() == ILoopStates::FS
            || m_dcdc_floating.getPFMState() == PFMStates::FO)   // + TODO: access to FGC4 fault chain open check
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::FS};
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toOff()
    {
        // Iloop goes to off
        if (m_dcdc_floating.getILoopState() == ILoopStates::OF)
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::OF};
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toStopping()
    {
        // from any state if Iloop is stopping
        if (m_dcdc_floating.getILoopState() == ILoopStates::SP)
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::SP};
        }

        // HMI request to stop
        if (checkHMIRequestStop())
        {
            // transition to stopping if current state is CH, CD, and DT
            if (getState() == DCDCFloatingVloopStates::CH || getState() == DCDCFloatingVloopStates::CD
                || getState() == DCDCFloatingVloopStates::DT)
            {
                return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::SP};
            }
            else   // FS otherwise
            {
                return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::FS};
            }
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toStarting()
    {
        // VS_RUN received from Iloop
        if (m_dcdc_floating.checkVSRunReceived())
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::ST};
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toBlocking()
    {
        // 0110 on inputs and V_out = 0 V
        if (checkOutputsReady() && m_dcdc_floating.getVout() < constants::v_out_threshold)
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::BK};
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toCharging()
    {
        // 'unblock' received from Iloop
        if (m_dcdc_floating.checkUnblockReceived())
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::CH};
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toCharged()
    {
        // from CH, Vdc > threshold
        if (getState() == DCDCFloatingVloopStates::CH
            && m_dcdc_floating.getVdc() >= constants::v_dc_floatings_charged_threshold)
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::CD};
        }

        // from DT, Vloop MASK set to 0
        if (getState() == DCDCFloatingVloopStates::DT && m_dcdc_floating.getVloopMask() == 0)
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::CD};
        }

        // no transition
        return {};
    }

    DCDCFloatingStateMachine::TransRes DCDCFloatingStateMachine::toDirect()
    {
        // Vloop MASK set to 1
        if (m_dcdc_floating.getVloopMask() == 1)
        {
            return DCDCFloatingStateMachine::TransRes{DCDCFloatingVloopStates::DT};
        }

        // no transition
        return {};
    }

}   // namespace user
