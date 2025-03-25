//! @file
//! @brief Definition of finite state machine for floating DCDCs
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"

namespace utils
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
            if (Vdc < vdc_min)   // DC bus is discharged
            {
                return TransRes{DCDCFloatingStates::fault_off};
            }
            return {};
        }

        TransRes toFaultStopping(const bool force_stop = false)
        {
            if (force_stop || gatewareFault() || interlock || I_loop.getState() == RegLoopStates::FS
                || pfm.getState() == PFMStates::FO)
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
            return TransRes{DCDCFloatingStates::stopping};
        }

        TransRes toStarting()
        {
            return TransRes{DCDCFloatingStates::starting};
        }

        TransRes toBlocking()
        {
            return TransRes{DCDCFloatingStates::blocking};
        }

        TransRes toCharging()
        {
            return TransRes{DCDCFloatingStates::charging};
        }

        TransRes toCharged()
        {
            return TransRes{DCDCFloatingStates::charged};
        }

        TransRes toDirect()
        {
            return TransRes{DCDCFloatingStates::direct};
        }
    };

}   // namespace user
