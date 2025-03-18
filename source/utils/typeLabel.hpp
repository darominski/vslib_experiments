//! @file
//! @brief File utility classes and functions to convert built-in supported types to std::string and std::string_view
//! representations.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "constants.hpp"
#include "typeTraits.hpp"

namespace fgc4::utils
{

    // Forward declaration of getTypeLabel function template
    template<typename T>
    auto getTypeLabel();

    // ************************************************************

    class TypeLabel
    {
      public:
        // Support for boolean type, otherwise would be caught by integral type
        std::string_view operator()(const bool) const
        {
            return constants::bool_type_label;
        }

        // ************************************************************

        // Support for enum types
        template<Enumeration T>
        std::string_view operator()(const T) const
        {
            return constants::enum_type_label;
        }

        // ************************************************************

        // Support for floating types
        template<Floating T>
        std::string_view operator()(const T) const
        {
            if constexpr (std::is_same<T, float>())   // from C++23: float32_t
            {
                return (constants::float32_type_label);
            }
            else if constexpr (std::is_same<T, double>())   // from C++23 float64_t
            {
                return (constants::float64_type_label);
            }
            else
            {   // the architecture is 64 bits, larger floats are not going to be supported (?)
                static_assert(always_false<T>, "Unsupported type.");
                return (constants::unsupported_type_label);   // should never reach here
            }
        }

        // ************************************************************

        // Support for integral types
        template<Integral T>
        std::string_view operator()(const T) const
        {
            // would it be nicer/more efficient to build the output string on the way?
            if constexpr (std::is_same_v<T, int64_t>)
            {
                return (constants::int64_type_label);
            }
            else if constexpr (std::is_same_v<T, uint64_t>)
            {
                return (constants::uint64_type_label);
            }
            else if constexpr (std::is_same_v<T, int32_t>)
            {
                return (constants::int32_type_label);
            }
            else if constexpr (std::is_same_v<T, uint32_t>)
            {
                return (constants::uint32_type_label);
            }
            else if constexpr (std::is_same_v<T, int16_t>)
            {
                return (constants::int16_type_label);
            }
            else if constexpr (std::is_same_v<T, uint16_t>)
            {
                return (constants::uint16_type_label);
            }
            else if constexpr (std::is_same_v<T, int8_t>)
            {
                return (constants::int8_type_label);
            }
            else if constexpr (std::is_same_v<T, uint8_t>)
            {
                return (constants::uint8_type_label);
            }
            else
            {
                static_assert(always_false<T>, "Unsupported type.");
                return (constants::unsupported_type_label);
            }
        }

        // ************************************************************

        // Support for std::string_view
        std::string_view operator()(const std::string&) const
        {
            return (constants::string_type_label);
        }

        // ************************************************************

        // Support for std::array types
        template<typename T, std::size_t N>
        std::string operator()(const std::array<T, N>&) const
        {
            return std::string(constants::array_type_prefix) + std::string(TypeLabel()(T{}));
        }

        // ************************************************************

        // fallback case, unsupported types
        template<typename T>
        std::string_view operator()(const T) const
        {
            static_assert(always_false<T>, "Unsupported type.");
            return (constants::unsupported_type_label);
        }
    };

    template<typename T>
    auto getTypeLabel()
    {
        return TypeLabel()(T{});
    }

}   // namespace fgc4::utils
