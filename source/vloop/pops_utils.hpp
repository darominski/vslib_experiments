//! @file
//! @brief Definition of a number of POPS-generic methods used throughout the POPS vloop software
//! @author Dominik Arominski

#pragma once

#include "constants.hpp"
#include "fsm.hpp"
#include "pops_constants.hpp"

namespace user
{

    enum class ILoopStates
    {
        FO,
        FS,
        SA,
        SP,
        RN,
        AB,
        AR,
        IL,
        EC,
        PD,
        PL,
        TC,
        OF,
        ST,
        TS,
        SB,
        BK,
        DT,
        CY,
    };

    enum class PFMStates
    {
        // TODO
        FO,
        ON
    };

    // bool check400VOpen()
    // {
    //     // TODO: checks whether the 400 V is open
    //     return false;
    // }

    // bool checkMCBOpen()
    // {
    //     // TODO: check if MCB is open
    //     return false;
    // }

    // bool checkMVOpen()
    // {
    //     // TODO: check if MV breaker is open
    //     return false;
    // }

    // bool checkLVOpen()
    // {
    //     // TODO: check if LV breaker is open
    //     return false;
    // }

    inline bool checkHMIRequestStop()
    {
        // TODO: check if the HMI force stop condition is fulfilled
        return false;
    }

    // bool checkHMIStop()
    // {
    //     // TODO: check if the HMI stop condition is fulfilled
    //     return false;
    // }

    // bool checkHMIRequestSB()
    // {
    //     // TODO: checks whether the HMI has requested a transition to standby (SB)
    //     return false;
    // }

    //! Checks for interlock, implementation TODO
    //!
    //! @return True for interlock issue, false otherwise
    inline bool checkInterlock()
    {
        // TODO: check the status of the interlock
        return false;
    }

    //! Checks for gateware faults, implementation TODO
    //!
    //! @return True if a fault has been found, false otherwise
    inline bool checkGatewareFault()
    {
        // TODO: check if conditions for a gateware fault are fulfilled
        return false;
    }

    //! Checks that outputs are equal to 0110.
    //!
    //! @return True if outputs are as expected, false otherwise
    inline bool checkOutputsReady()
    {
        // TODO: check if outputs == 0110
        return true;
    }

}   // namespace user