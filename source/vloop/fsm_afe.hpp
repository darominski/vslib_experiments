//! @file
//! @brief Definition of finite state machine for Active Front-Ends
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.hpp"
#include "pops_utils.hpp"

namespace user
{
    enum class AFEVloopStates
    {
        FO,   // fault_off
        FS,   // fault stopping
        OF,   // off
        SP,   // stopping
        PH,   // precharging
        PD,   // precharged
        ON    // on
    };

    class AFEStateMachine
    {
        using StateMachine = ::utils::Fsm<AFEVloopStates, AFEStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<AFEVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<AFEVloopStates> (AFEStateMachine::*)();

      public:
        AFEStateMachine()
            : m_fsm(*this, AFEVloopStates::FO)
        {
            // Initialize handles for the i_loop state, vdc measurement, gateware status, interlock status, and the PFM
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
        void onPrecharging()
        {
        }
        void onPrecharged()
        {
        }
        void onOn()
        {
        }

        TransRes toFaultOff()
        {
            // DC bus is discharged, MCB should be open, and MV and LV breakers should be open
            if (getVdc() < constants::v_dc_min && checkMCBOpen() && checkMVOpen() && checkLVOpen())
            {
                return TransRes{AFEVloopStates::FO};
            }
            return {};
        }

        TransRes toFaultStopping()
        {
            if (checkGatewareFault() || checkInterlock() || i_loop.getState() == IloopStates::FS
                || pfm.getState() == PFMStates::FO
                || (getState() == AFEVloopStates::PD && i_loop.getState() == IloopStates::SP))
            {
                return TransRes{AFEVloopStates::FS};
            }
            return {};
        }

        TransRes toOff()
        {
            if (i_loop.getState() == IloopStates::OF)
            {
                return TransRes{AFEVloopStates::OF};
            }
            return {};
        }

        //! Transition to the stopping state.
        //!
        //! @param force_stop Force stopping of the DCDC, e.g. from a HMI request
        TransRes toStopping()
        {
            if (i_loop.getState() == IloopStates::SP)
            {
                return TransRes{AFEVloopStates::SP};
            }
            return {};
        }

        TransRes toPrecharging()
        {
            if (checkVSRunReceived())
            {
                return TransRes{AFEVloopStates::PH};
            }
            return {};
        }

        TransRes toPrecharged()
        {
            if (!checkMCBOpen() && check400VOpen() && getVdcCharger() > constants::v_dc_charger_min)
            {
                return TransRes{AFEVloopStates::PD};
            }
            return {};
        }

        TransRes toOn()
        {
            if (checkUnblockReceived())
            {
                return TransRes{AFEVloopStates::ON};
            }
            return {};
        }

      private:
        //! Returns the Vdc value of the DCDC Charger
        //!
        //! @return Vdc charger value in V
        double getVdcCharger()
        {
            // TODO: get Vdc value of Floating DCDC
            return 0.0;
        }

        //! Returns the Vdc value
        //!
        //! Vdc value in V
        double getVdc()
        {
            // TODO: get V dc
            return 0.0;
        }
    };
}   // namespace user
