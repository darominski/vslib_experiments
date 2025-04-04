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
    enum class DCDCFloatingStates
    {
        fault_off,
        fault_stopping,
        off,
        stopping,
        starting,
        blocking,
        charging,
        charged,
        direct
    };

    class DCDCFloatingFSM
    {
        using StateMachine = ::utils::Fsm<DCDCFloatingStates, DCDCFloatingFSM, false>;

        using TransRes = ::utils::FsmTransitionResult<DCDCFloatingStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<DCDCFloatingStates> (DCDCFloatingFSM::*)();

      public:
        DCDCFloatingFSM()
            : m_fsm(*this, DCDCFloatingStates::fault_off)
        {
            // Initialize handles for the I_loop state, gateware status, interlock status, and PFM state

            // CAUTION: The order of transition method matters
            // clang-format off
            m_fsm.addState(DCDCFloatingStates::fault_off,      &DCDCFloatingFSM::onFaultOff,      {&DCDCFloatingFSM::toOff});
            m_fsm.addState(DCDCFloatingStates::fault_stopping, &DCDCFloatingFSM::onFaultStopping, {&DCDCFloatingFSM::toFaultOff});
            m_fsm.addState(DCDCFloatingStates::off,            &DCDCFloatingFSM::onOff,           {&DCDCFloatingFSM::toFaultStopping, &DCDCFloatingFSM::toStarting});
            m_fsm.addState(DCDCFloatingStates::stopping,       &DCDCFloatingFSM::onStopping,      {&DCDCFloatingFSM::toFaultStopping, &DCDCFloatingFSM::toOff});
            m_fsm.addState(DCDCFloatingStates::starting,       &DCDCFloatingFSM::onStarting,      {&DCDCFloatingFSM::toFaultStopping, &DCDCFloatingFSM::toStopping, &DCDCFloatingFSM::toBlocking});
            m_fsm.addState(DCDCFloatingStates::blocking,       &DCDCFloatingFSM::onBlocking,      {&DCDCFloatingFSM::toFaultStopping, &DCDCFloatingFSM::toStopping, &DCDCFloatingFSM::toCharging});
            m_fsm.addState(DCDCFloatingStates::charging,       &DCDCFloatingFSM::onCharging,      {&DCDCFloatingFSM::toFaultStopping, &DCDCFloatingFSM::toStopping, &DCDCFloatingFSM::toCharged});
            m_fsm.addState(DCDCFloatingStates::charged,        &DCDCFloatingFSM::onCharged,       {&DCDCFloatingFSM::toFaultStopping, &DCDCFloatingFSM::toStopping, &DCDCFloatingFSM::toDirect});
            m_fsm.addState(DCDCFloatingStates::direct,         &DCDCFloatingFSM::onDirect,        {&DCDCFloatingFSM::toFaultStopping, &DCDCFloatingFSM::toStopping, &DCDCFloatingFSM::toCharged});
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
            if (getVdc() < constants::v_dc_min)   // DC bus is discharged
            {
                return TransRes{DCDCFloatingStates::fault_off};
            }
            return {};
        }

        TransRes toFaultStopping()
        {
            if (checkHMIForceStop() || checkGatewareFault() || checkInterlock()
                || I_loop.getState() == RegLoopStates::FS || pfm.getState() == PFMStates::FO)
            {
                return TransRes{DCDCFloatingStates::fault_stopping};
            }
            return {};
        }

        TransRes toOff()
        {
            if (I_loop.getState() == RegLoopStates::OF)
            {
                return TransRes{DCDCFloatingStates::off};
            }
            return {};
        }

        TransRes toStopping()
        {
            if (I_loop.getState() == RegLoopStates::SP
                || (checkHMIForceStop()
                    && (getState() == DCDCFloatingStates::charging || getState() == DCDCFloatingStates::charged
                        || getState() == DCDCFloatingStates::direct)))
            {
                return TransRes{DCDCFloatingStates::stopping};
            }
            return {};
        }

        TransRes toStarting()
        {
            if (checkVSRunReceived())
            {
                return TransRes{DCDCFloatingStates::starting};
            }
            return {};
        }

        TransRes toBlocking()
        {
            if (checkOutputsReady() && getVout() < constants::v_out_threshold)
            {
                return TransRes{DCDCFloatingStates::blocking};
            }
            return {};
        }

        TransRes toCharging()
        {
            if (checkUnblockReceived())
            {
                return TransRes{DCDCFloatingStates::charging};
            }
            return {};
        }

        TransRes toCharged()
        {
            if ((getState() == DCDCFloatingStates::charging && getVdc() >= constants::v_dc_floatings_charged_threshold)
                || (getState() == DCDCFloatingStates::direct && getVloopMask() == 0))
            {
                return TransRes{DCDCFloatingStates::charged};
            }
            return {};
        }

        TransRes toDirect()
        {
            if (getVloopMask() == 1)
            {
                return TransRes{DCDCFloatingStates::direct};
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
