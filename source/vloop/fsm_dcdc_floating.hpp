//! @file
//! @brief Definition of finite state machine for floating DCDCs
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.h"
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

    class DCDCFloatingStateMachine
    {
        using StateMachine = ::utils::Fsm<DCDCFloatingVloopStates, DCDCFloatingStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<DCDCFloatingVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<DCDCFloatingVloopStates> (DCDCFloatingStateMachine::*)();

      public:
        DCDCFloatingStateMachine()
            : m_fsm(*this, DCDCFloatingVloopStates::FO)
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
            // send VS power ON
            // send VS ready
        }
        void onCharging()
        {
        }
        void onCharged()
        {
        }
        void onDirect()
        {
        }

        TransRes toFaultOff()
        {
            if (getVdc() < constants::v_dc_min)   // DC bus is disCD
            {
                return TransRes{DCDCFloatingVloopStates::FO};
            }
            return {};
        }

        TransRes toFaultStopping()
        {
            if (checkHMIRequestStop() || checkGatewareFault() || checkInterlock()
                || i_loop.getState() == IloopStates::FS || pfm.getState() == PFMStates::FO)
            {
                return TransRes{DCDCFloatingVloopStates::FS};
            }
            return {};
        }

        TransRes toOff()
        {
            if (i_loop.getState() == IloopStates::OF)
            {
                return TransRes{DCDCFloatingVloopStates::OF};
            }
            return {};
        }

        TransRes toStopping()
        {
            // from any state if Iloop is stopping
            if (i_loop.getState() == IloopStates::SP)
            {
                return TransRes{DCDCFloatingVloopStates::SP};
            }

            // HMI request to stop, valid for CH, CD, and DT
            if (checkHMIRequestStop()
                && (getState() == DCDCFloatingVloopStates::CH || getState() == DCDCFloatingVloopStates::CD
                    || getState() == DCDCFloatingVloopStates::DT))
            {
                return TransRes{DCDCFloatingVloopStates::SP};
            }

            return {};
        }

        TransRes toStarting()
        {
            if (checkVSRunReceived())
            {
                return TransRes{DCDCFloatingVloopStates::ST};
            }
            return {};
        }

        TransRes toBlocking()
        {
            if (checkOutputsReady() && getVout() < constants::v_out_threshold)
            {
                return TransRes{DCDCFloatingVloopStates::BK};
            }
            return {};
        }

        TransRes toCharging()
        {
            if (checkUnblockReceived())
            {
                return TransRes{DCDCFloatingVloopStates::CH};
            }
            return {};
        }

        TransRes toCharged()
        {
            if ((getState() == DCDCFloatingVloopStates::CH && getVdc() >= constants::v_dc_floatings_charged_threshold)
                || (getState() == DCDCFloatingVloopStates::DT && getVloopMask() == 0))
            {
                return TransRes{DCDCFloatingVloopStates::CD};
            }
            return {};
        }

        TransRes toDirect()
        {
            if (getVloopMask() == 1)
            {
                return TransRes{DCDCFloatingVloopStates::DT};
            }
            return {};
        }

        double getVdc()
        {
            // TODO: get V dc
            return 0.0;
        }

        double getVout()
        {
            // TODO: get V out
            return 0.0;
        }
    };

}   // namespace user
