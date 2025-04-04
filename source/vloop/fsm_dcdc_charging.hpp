//! @file
//! @brief Definition of finite state machine for charger DCDCs
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.h"
#include "pops_utils.hpp"

namespace user
{
    enum class DCDCChargerStates
    {
        fault_off,
        fault_stopping,
        off,
        stopping,
        starting,
        blocking,
        direct
    };

    class DCDChargerFSM
    {
        using StateMachine = ::utils::Fsm<DCDCChargerStates, DCDChargerFSM, false>;

        using TransRes = ::utils::FsmTransitionResult<DCDCChargerStates>;

        using StateFunc = std::function<void(void)>;

        //! Convenience alias representing pointer to a member function of the Parent class, for a transition function.
        using TransitionFunc = ::utils::FsmTransitionResult<DCDCChargerStates> (DCDChargerFSM::*)();

      public:
        DCDChargerFSM()
            : m_fsm(*this, DCDCChargerStates::fault_off)
        {
            // Initialize handles for the I_loop state, vdc measurement, gateware status, interlock status, and the PFM
            // status

            // CAUTION: The order of transition method matters
            // clang-format off
            m_fsm.addState(DCDCChargerStates::fault_off,      &DCDChargerFSM::onFaultOff,      {&DCDChargerFSM::toOff});
            m_fsm.addState(DCDCChargerStates::fault_stopping, &DCDChargerFSM::onFaultStopping, {&DCDChargerFSM::toFaultOff});
            m_fsm.addState(DCDCChargerStates::off,            &DCDChargerFSM::onOff,           {&DCDChargerFSM::toFaultStopping, &DCDChargerFSM::toStarting});
            m_fsm.addState(DCDCChargerStates::stopping,       &DCDChargerFSM::onStopping,      {&DCDChargerFSM::toFaultStopping, &DCDChargerFSM::toOff});
            m_fsm.addState(DCDCChargerStates::starting,       &DCDChargerFSM::onStarting,      {&DCDChargerFSM::toFaultStopping, &DCDChargerFSM::toStopping, &DCDChargerFSM::toBlocking});
            m_fsm.addState(DCDCChargerStates::blocking,       &DCDChargerFSM::onBlocking,      {&DCDChargerFSM::toFaultStopping, &DCDChargerFSM::toStopping, &DCDChargerFSM::toDirect});
            m_fsm.addState(DCDCChargerStates::direct,         &DCDChargerFSM::onDirect,        {&DCDChargerFSM::toFaultStopping, &DCDChargerFSM::toStopping, &DCDChargerFSM::toBlocking});
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
                return TransRes{DCDCChargerStates::fault_off};
            }
            return {};
        }

        TransRes toFaultStopping(const bool force_stop = false)
        {
            if (force_stop || checkGatewareFault() || checkInterlock() || I_loop.getState() == RegLoopStates::FS
                || pfm.getState() == PFMStates::FO)
            {
                return TransRes{DCDCChargerStates::fault_stopping};
            }
            return {};
        }

        TransRes toOff()
        {
            if (I_loop.getState() == RegLoopStates::OF)
            {
                return TransRes{DCDCChargerStates::off};
            }
            return {};
        }

        //! Transition to the stopping state.
        //!
        //! @param force_stop Force stopping of the DCDC, e.g. from a HMI request
        TransRes toStopping(const bool force_stop = false)
        {
            if (I_loop.getState() == RegLoopStates::SP || force_stop)
            {
                return TransRes{DCDCChargerStates::stopping};
            }
            return {};
        }

        TransRes toStarting()
        {
            if (checkVSRunReceived())
            {
                return TransRes{DCDCChargerStates::starting};
            }
            return {};
        }

        TransRes toBlocking()
        {
            if ((getState() == DCDCChargerStates::starting && checkOutputsReady()
                 && getVout() <= constants::v_out_threshold)
                || (getState() == DCDCChargerStates::direct && checkAllFloatingsInBK()
                    && getVdcFloatings() < constants::v_dc_floatings_min_threshold))
            {
                return TransRes{DCDCChargerStates::blocking};
            }
            return {};
        }

        TransRes toDirect()
        {
            if (checkUnblockReceived() && getVloopMask())
            {
                return TransRes{DCDCChargerStates::direct};
            }
            return {};
        }

        //! Checks if all Floating DCDC are in blocking (BK) state.
        //!
        //! @return True if all Floating DCDCs are in BK, false otherwise
        bool checkAllFloatingsInBK()
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
