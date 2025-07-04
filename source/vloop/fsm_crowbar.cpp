#include "fsm_crowbar.hpp"
#include "user.hpp"

namespace user
{
    CWBStateMachine::CWBStateMachine(Converter& crowbar)
        : m_fsm(*this, CWBVloopStates::FO),
          m_crowbar(crowbar)
    {
        // obtain handles for the i_loop state and the intertrip light state

        // CAUTION: The order of transition method matters
        // clang-format off
        m_fsm.addState(CWBVloopStates::FO, &CWBStateMachine::onFaultOff, {&CWBStateMachine::toOn});
        m_fsm.addState(CWBVloopStates::ON, &CWBStateMachine::onOn,       {&CWBStateMachine::toFaultOff});
        // clang-format on
    }

    void CWBStateMachine::update()
    {
        m_fsm.update();
    }

    [[nodiscard]] CWBVloopStates CWBStateMachine::getState() const noexcept
    {
        return m_fsm.getState();
    }

    void CWBStateMachine::onFaultOff()
    {
        // open the safety chain?
        // TODO: set the PFM communication to trigger the emergency shutdown. There will be a dedicated pin.
        // hold the thyristor ON and hold the intertrip light OFF.
    }

    void CWBStateMachine::onOn()
    {
        // hold the thyristor OFF and hold the intertrip light ON.
    }

    CWBStateMachine::TransRes CWBStateMachine::toOn()
    {
        // std::cout << m_crowbar.m_i_loop_state << " " << m_crowbar.checkVSRunReceived() << '\n';
        if (m_crowbar.m_i_loop_state == ILoopStates::ST && m_crowbar.checkVSRunReceived())
        {
            return TransRes{CWBVloopStates::ON};
        }
        return {};
    }

    CWBStateMachine::TransRes CWBStateMachine::toFaultOff()
    {
        // if (!m_crowbar.checkIntertripLight() || m_crowbar.m_i_loop_state == ILoopStates::FO)
        if (m_crowbar.m_i_loop_state == ILoopStates::FO || m_crowbar.m_i_loop_state == ILoopStates::FS
            || (m_crowbar.m_fault == 1))
        {
            return TransRes{CWBVloopStates::FO};
        }
        return {};
    }

}   // namespace user
