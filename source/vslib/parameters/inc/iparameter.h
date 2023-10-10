//! @file
//! @brief File containing non-templated base class for Parameter.
//! @author Dominik Arominski

#pragma once

#include <optional>
#include <string>

#include "errorMessage.h"
#include "nlohmann/json.hpp"
#include "staticJson.h"

namespace vslib::parameters
{
    class IParameter
    {
      public:
        IParameter(std::string_view name)
            : m_name(name)
        {
        }

        virtual ~IParameter() = default;

        [[nodiscard]] std::string_view getName() const
        {
            return m_name;
        }
        virtual std::optional<fgc4::utils::Error> setJsonValue(const fgc4::utils::StaticJson&) = 0;
        virtual nlohmann::json                    serialize() const noexcept                   = 0;
        virtual void                              synchroniseWriteBuffer()                     = 0;
        virtual void                              synchroniseReadBuffers()                     = 0;

      protected:
        const std::string m_name;   // Unique ID indicating component type, its name and the variable name
    };
}
