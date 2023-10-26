//! @file
//! @brief File containing abstract interface for Parameter.
//! @author Dominik Arominski

#pragma once

#include <optional>

#include "nonCopyableNonMovable.h"
#include "staticJson.h"
#include "warningMessage.h"

namespace vslib::parameters
{
    class IParameter : public NonCopyableNonMovable
    {
      public:
        virtual ~IParameter() = default;

        virtual std::optional<fgc4::utils::Warning> setJsonValue(const fgc4::utils::StaticJson&) = 0;
        virtual fgc4::utils::StaticJson             serialize() const noexcept                   = 0;
        virtual void                                synchroniseWriteBuffer()                     = 0;
        virtual void                                synchroniseReadBuffers()                     = 0;
    };
}   // namespace vslib::parameters
