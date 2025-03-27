//! @file
//! @brief Definition of Finite state machine for a crowbar
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"

namespace user
{
    enum class CrowbarStates
    {
        fault_off,
        on
    };

    class CrowbarFSM
    {
        using StateMachine = ::utils::Fsm<CrowbarStates, CrowbarFSM, false>;

        using TransRes = ::utils::FsmTransitionResult<CrowbarStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<CrowbarStates> (CrowbarFSM::*)();

      public:
        CrowbarFSM()
            : m_fsm(*this, CrowbarStates::fault_off)
        {
            // obtain handles for the I_loop state and the intertrip light state

            // CAUTION: The order of transition method matters
            // clang-format off
            m_fsm.addState(CrowbarStates::fault_off, &CrowbarFSM::onFaultOff, {&CrowbarFSM::toOn});
            m_fsm.addState(CrowbarStates::on,        &CrowbarFSM::onOn,       {&CrowbarFSM::toFaultOff});
            // clang-format on
        }

        void update()
        {
            m_fsm.update();
        }

        [[nodiscard]] const auto getState() const noexcept
        {
            return m_fsm.getState();
        }

      private:
        StateMachine m_fsm;

        void onFaultOff()
        {
            // open the safety chain?
        }

        void onOn()
        {
        }

        TransRes toOn()
        {
            if (I_loop sent VS_RUN)
            {
                return TransRes{CrowbarStates::on};
            }
            return {};
        }

        TransRes toFaultOff()
        {
            if (!intertrip_light || I_loop.getState() == FO)
            {
                return TransRes{CrowbarStates::fault_off};
            }
            return {};
        }
    };

}   // namespace user
