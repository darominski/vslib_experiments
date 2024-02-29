//! @file
//! @brief File containing abstract interface for Parameter.
//! @author Dominik Arominski

#pragma once

#include <optional>

#include "nonCopyableNonMovable.h"
#include "staticJson.h"
#include "warningMessage.h"

namespace vslib
{
    class ParameterSerializer;   // forward declaration of serialization visitor

    class IParameter : public NonCopyableNonMovable
    {
      public:
        virtual ~IParameter() = default;

        virtual std::string_view                    getName() const noexcept                             = 0;
        virtual std::optional<fgc4::utils::Warning> setJsonValue(const fgc4::utils::StaticJson&)         = 0;
        virtual fgc4::utils::StaticJson             serialize(const ParameterSerializer&) const noexcept = 0;
        virtual void                                synchroniseBuffers()                                 = 0;
    };
}   // namespace vslib
