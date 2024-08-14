//! Single-include holding everything from the VSlib

#pragma once

//! Generic Parameter
#include "parameter.h"

//! Generic Component
#include "component.h"
#include "componentArray.h"
#include "converter.h"
#include "rootComponent.h"

//! Transforms
#include "alphaBetaZeroToDq0Transform.h"
#include "clarkeTransform.h"
#include "parkTransform.h"

//! Controllers
#include "pid.h"
#include "rst.h"

//! Filters
#include "boxFilter.h"
#include "firFilter.h"
#include "iirFilter.h"

//! Limits
#include "limitIntegral.h"
#include "limitRange.h"
#include "limitRate.h"
#include "limitRms.h"

//! Look-up tables and related
#include "cosLookupTable.h"
#include "functionGenerator.h"
#include "lookupTable.h"
#include "periodicLookupTable.h"
#include "sinLookupTable.h"

//! Interrupts
#include "peripheralInterrupt.h"
#include "timerInterrupt.h"

//! Utils
#include "fixedPointType.h"