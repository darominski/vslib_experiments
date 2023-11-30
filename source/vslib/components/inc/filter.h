//! @file
//! @brief Defines the base class for filters.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string>

#include "component.h"

namespace vslib
{
    class Filter : public Component
    {
      public:
        Filter(std::string_view type, std::string_view name, Component* parent = nullptr, double max_input_value = 1e6)
            : Component(type, name, parent),
              m_max_input_value(max_input_value),
              m_float_to_integer(std::numeric_limits<int32_t>::max() / max_input_value),
              m_integer_to_float(max_input_value / std::numeric_limits<int32_t>::max())
        {
        }

        virtual double filter(double) = 0;

        [[nodiscard]] auto getMaxInputValue() const noexcept
        {
            return m_max_input_value;
        }

      protected:
        double const m_max_input_value;
        double const m_float_to_integer;
        double const m_integer_to_float;
    };
}   // namespace vslib