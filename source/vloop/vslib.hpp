//! Single-include holding everything from the VSlib

#pragma once

//! Generic Parameter
#include "parameter.hpp"

//! Generic Component
#include "component.hpp"
#include "componentArray.hpp"
#include "iconverter.hpp"
#include "rootComponent.hpp"

//! Transforms
#include "abcToAlphaBetaTransform.hpp"
#include "abcToDq0Transform.hpp"
#include "alphaBetaToAbcTransform.hpp"
#include "alphaBetaToDq0Transform.hpp"
#include "dq0ToAbcTransform.hpp"
#include "dq0ToAlphaBetaTransform.hpp"
#include "instantaneousPowerThreePhase.hpp"

//! Controllers
#include "pid.hpp"
#include "rst.hpp"

//! Current balancing
#include "srfPll.hpp"

//! Filters
#include "boxFilter.hpp"
#include "firFilter.hpp"
#include "iirFilter.hpp"

//! Limits
#include "limitIntegral.hpp"
#include "limitRange.hpp"
#include "limitRate.hpp"
#include "limitRms.hpp"

//! Look-up tables and related
#include "cosLookupTable.hpp"
#include "functionGenerator.hpp"
#include "lookupTable.hpp"
#include "periodicLookupTable.hpp"
#include "sinLookupTable.hpp"

//! Interrupts
#include "peripheralInterrupt.hpp"
#include "timerInterrupt.hpp"

//! Utils
#include "fixedPointType.hpp"