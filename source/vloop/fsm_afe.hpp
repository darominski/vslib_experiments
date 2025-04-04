//! @file
//! @brief Definition of finite state machine for Active Front-Ends
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.h"
#include "pops_utils.hpp"

namespace user
{
    enum class AFEStates
    {
        fault_off,
        fault_stopping,
        off,
        stopping,
        precharging,
        precharged,
        on
    };

    class AFEFSM
    {
        using StateMachine = ::utils::Fsm<AFEStates, AFEFSM, false>;

        using TransRes = ::utils::FsmTransitionResult<AFEStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<AFEStates> (AFEFSM::*)();

      public:
        AFEFSM()
            : m_fsm(*this, AFEStates::fault_off)
        {
            // Initialize handles for the I_loop state, vdc measurement, gateware status, interlock status, and the PFM
            // status

            // CAUTION: The order of transition method matters
            // clang-format off
            m_fsm.addState(AFEStates::fault_off,      &AFEFSM::onFaultOff,      {&AFEFSM::toOff});
            m_fsm.addState(AFEStates::fault_stopping, &AFEFSM::onFaultStopping, {&AFEFSM::toFaultOff});
            m_fsm.addState(AFEStates::off,            &AFEFSM::onOff,           {&AFEFSM::toFaultStopping, &AFEFSM::toPrecharging});
            m_fsm.addState(AFEStates::stopping,       &AFEFSM::onStopping,      {&AFEFSM::toFaultStopping, &AFEFSM::toOff});
            m_fsm.addState(AFEStates::precharging,    &AFEFSM::onPrecharging,   {&AFEFSM::toFaultStopping, &AFEFSM::toPrecharged});
            m_fsm.addState(AFEStates::precharged,     &AFEFSM::onPrecharged,    {&AFEFSM::toFaultStopping, &AFEFSM::toOn});
            m_fsm.addState(AFEStates::on,             &AFEFSM::onOn,            {&AFEFSM::toFaultStopping, &AFEFSM::toStopping});
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
                return TransRes{AFEStates::fault_off};
            }
            return {};
        }

        TransRes toFaultStopping(const bool force_stop = false)
        {
            if (force_stop || checkGatewareFault() || checkInterlock() || I_loop.getState() == RegLoopStates::FS
                || pfm.getState() == PFMStates::FO
                || (getState() == AFEStates::precharged && I_loop.getState() == RegLoopStates::SP))
            {
                return TransRes{AFEStates::fault_stopping};
            }
            return {};
        }

        TransRes toOff()
        {
            if (I_loop.getState() == RegLoopStates::OF)
            {
                return TransRes{AFEStates::off};
            }
            return {};
        }

        //! Transition to the stopping state.
        //!
        //! @param force_stop Force stopping of the DCDC, e.g. from a HMI request
        TransRes toStopping(const bool force_stop = false)
        {
            if (force_stop || I_loop.getState() == RegLoopStates::SP)
            {
                return TransRes{AFEStates::stopping};
            }
            return {};
        }

        TransRes toPrecharging()
        {
            if (checkVSRunReceived())
            {
                return TransRes{AFEStates::precharging};
            }
            return {};
        }

        TransRes toPrecharged()
        {
            if (!checkMCBOpen() && check400VOpen() && getVdcCharger() > constants::v_dc_charger_min)
            {
                return TransRes{AFEStates::precharged};
            }
            return {};
        }

        TransRes toOn()
        {
            if (checkUnblockReceived())
            {
                return TransRes{AFEStates::on};
            }
            return {};
        }

      private:
        double getVdcCharger()
        {
            // TODO: get Vdc value of Floating DCDC
            return 0.0;
        }

        double getVdc()
        {
            // TODO: get V dc
            return 0.0;
        }
    };
}   // namespace user
