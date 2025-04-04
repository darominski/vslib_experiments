//! @file
//! @brief Definition of finite state machine for charger DCDCs
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.hpp"
#include "pops_utils.hpp"

namespace user
{
    enum class DCDCChargerVloopStates
    {
        FO,   // fault off
        FS,   // fault stopping
        OF,   // off
        SP,   // stopping
        ST,   // starting
        BK,   // blocking
        DT    // direct
    };

    class DCDCChargerStateMachine
    {
        using StateMachine = ::utils::Fsm<DCDCChargerVloopStates, DCDCChargerStateMachine, false>;

        using TransRes = ::utils::FsmTransitionResult<DCDCChargerVloopStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<DCDCChargerVloopStates> (DCDCChargerStateMachine::*)();

      public:
        DCDCChargerStateMachine()
            : m_fsm(*this, DCDCChargerVloopStates::FO)
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
            if (getVdc() < constants::v_dc_min)   // DC bus is discharged
            {
                return TransRes{DCDCChargerVloopStates::FO};
            }
            return {};
        }

        TransRes toFaultStopping()
        {
            if (checkGatewareFault() || checkInterlock() || i_loop.getState() == IloopStates::FS
                || pfm.getState() == PFMStates::FO)
            {
                return TransRes{DCDCChargerVloopStates::FS};
            }
            return {};
        }

        TransRes toOff()
        {
            if (i_loop.getState() == IloopStates::OF)
            {
                return TransRes{DCDCChargerVloopStates::OF};
            }
            return {};
        }

        //! Transition to the SP state.
        TransRes toStopping()
        {
            if (i_loop.getState() == IloopStates::SP || checkHMIRequestStop())
            {
                return TransRes{DCDCChargerVloopStates::SP};
            }
            return {};
        }

        TransRes toStarting()
        {
            if (checkVSRunReceived())
            {
                return TransRes{DCDCChargerVloopStates::ST};
            }
            return {};
        }

        TransRes toBlocking()
        {
            // from ST
            if (getState() == DCDCChargerVloopStates::ST && checkOutputsReady()
                && getVout() <= constants::v_out_threshold)
            {
                return TransRes{DCDCChargerVloopStates::BK};
            }

            // from DT
            if (getState() == DCDCChargerVloopStates::DT && checkAllFloatingVloopInBK()
                && getVdcFloatings() < constants::v_dc_floatings_min_threshold)
            {
                return TransRes{DCDCChargerVloopStates::BK};
            }
            return {};
        }

        TransRes toDirect()
        {
            if (checkUnblockReceived() && getVloopMask())
            {
                return TransRes{DCDCChargerVloopStates::DT};
            }
            return {};
        }

        //! Checks if all Floating DCDC are in BK (BK) state.
        //!
        //! @return True if all Floating DCDCs are in BK, false otherwise
        bool checkAllFloatingVloopInBK()
        {
            // TODO: loop over all Floating DCDC and check their state. If all are in BK, return true, false otherwise.
            return false;
        }

        //! Returns the Vdc value of the connected Floating DCDC.
        //!
        //! @return Vdc value of the connected Floating DCDC, in V.
        double getVdcFloatings()
        {
            // TODO: get Vdc value of Floating DCDC
            return 0.0;
        }

        //! Returns the Vout.
        //!
        //! @return Vout value, in V.
        double getVout()
        {
            // TODO: get V out
            return 0.0;
        }

        //! Returns the Vdc.
        //!
        //! @return Vdc value, in V.
        double getVdc()
        {
            // TODO: get V dc
            return 0.0;
        }
    };

}   // namespace user
