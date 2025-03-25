//! @file
//! @brief Definition of finite state machine for floating DCDCs
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"

namespace utils
{
    enum class DCDCChargingStates
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

    class DCDChargingFSM
    {
        using StateMachine = ::utils::Fsm<DCDCChargingStates, DCDChargingFSM, false>;

        using TransRes = ::utils::FsmTransitionResult<DCDCChargingStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<DCDCChargingStates> (DCDChargingFSM::*)();

      public:
        DCDChargingFSM()
            : m_fsm(*this, DCDCChargingStates::fault_off)
        {
            // CAUTION: The order of transition method matters

            // clang-format off
            m_fsm.addState(DCDCChargingStates::fault_off,      &DCDChargingFSM::onFaultOff,      {&DCDChargingFSM::toOff});
            m_fsm.addState(DCDCChargingStates::fault_stopping, &DCDChargingFSM::onFaultStopping, {&DCDChargingFSM::toFaultOff});
            m_fsm.addState(DCDCChargingStates::off,            &DCDChargingFSM::onOff,           {&DCDChargingFSM::toFaultStopping, &DCDChargingFSM::toStarting});
            m_fsm.addState(DCDCChargingStates::stopping,       &DCDChargingFSM::onStopping,      {&DCDChargingFSM::toFaultStopping, &DCDChargingFSM::toOff});
            m_fsm.addState(DCDCChargingStates::starting,       &DCDChargingFSM::onStarting,      {&DCDChargingFSM::toFaultStopping, &DCDChargingFSM::toStopping, &DCDChargingFSM::toBlocking});
            m_fsm.addState(DCDCChargingStates::blocking,       &DCDChargingFSM::onBlocking,      {&DCDChargingFSM::toFaultStopping, &DCDChargingFSM::toStopping});
            m_fsm.addState(DCDCChargingStates::charging,       &DCDChargingFSM::onCharging,      {&DCDChargingFSM::toFaultStopping, &DCDChargingFSM::toStopping, &DCDChargingFSM::toCharged});
            m_fsm.addState(DCDCChargingStates::charged,        &DCDChargingFSM::onCharged,       {&DCDChargingFSM::toFaultStopping, &DCDChargingFSM::toStopping, &DCDChargingFSM::toDirect});
            m_fsm.addState(DCDCChargingStates::direct,         &DCDChargingFSM::onDirect,        {&DCDChargingFSM::toFaultStopping, &DCDChargingFSM::toStopping, &DCDChargingFSM::toCharged});
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
        void onCharged()
        {
        }
        void onDirect()
        {
        }

        TransRes toFaultOff()
        {
            if (Vdc < vdc_min)   // DC bus is discharged
            {
                return TransRes{DCDCChargingStates::fault_off};
            }
            return {};
        }

        TransRes toFaultStopping(const bool force_stop = false)
        {
            if (force_stop || gatewareFault() || interlock || I_loop.getState() == RegLoopStates::FS
                || pfm.getState() == PFMStates::FO)
            {
                return TransRes{DCDCChargingStates::fault_stopping};
            }
            return {};
        }

        TransRes toOff()
        {
            if (I_loop.getState() == RegLoopStates::OF)
            {
                return TransRes{DCDCChargingStates::off};
            }
            return {};
        }

        TransRes toStopping()
        {
            return TransRes{DCDCChargingStates::stopping};
        }

        TransRes toStarting()
        {
            return TransRes{DCDCChargingStates::starting};
        }

        TransRes toBlocking()
        {
            return TransRes{DCDCChargingStates::blocking};
        }

        TransRes toCharging()
        {
            return TransRes{DCDCChargingStates::charging};
        }

        TransRes toCharged()
        {
            return TransRes{DCDCChargingStates::charged};
        }

        TransRes toDirect()
        {
            return TransRes{DCDCChargingStates::direct};
        }
    };

}   // namespace user
