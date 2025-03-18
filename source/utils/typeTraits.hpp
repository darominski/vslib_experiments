//! @file
//! @brief File with concept and useful type-traits definitions.
//! @author Dominik Arominski

#pragma once

#include <array>
#include <concepts>
#include <string>
#include <type_traits>

namespace fgc4::utils
{
    // ************************************************************

    // helper concept declarations
    template<typename T>
    concept Integral = std::is_integral_v<T>;

    template<typename T>
    concept Floating = std::is_floating_point_v<T>;

    template<typename T>
    concept NumericScalar = Floating<T> || Integral<T>;

    // helper definitions for std::array parameter types
    template<typename T>
    struct IsStdArray : std::false_type
    {
    };

    template<typename T, std::size_t N>
    struct IsStdArray<std::array<T, N>> : std::true_type
    {
    };

    template<typename T>
    concept StdArray = IsStdArray<T>::value;

    template<typename T>
    concept NumericArray = StdArray<T> && NumericScalar<typename T::value_type>;

    template<typename T>
    concept Numeric = NumericScalar<T> || NumericArray<T>;

    template<typename T>
    concept Enumeration = std::is_enum_v<T>;

    template<typename T>
    concept NonNumeric = Enumeration<T> || (!Numeric<T>);

    // helper definitions for std::string type
    template<typename T>
    concept ToStringable = requires(T t) {
                               {
                                   std::to_string(t)
                                   } -> std::same_as<std::string>;
                           };

    template<typename T>
    concept StringCastable = requires(T t) {
                                 {
                                     std::string(t)
                                     } -> std::same_as<std::string>;
                             };

    template<typename T, typename = void>
    struct isString
    {
        static const bool value = false;
    };

    // Also covers strings with custom allocators
    template<typename T, class Traits, class Alloc>
    struct isString<std::basic_string<T, Traits, Alloc>, void>
    {
        static const bool value = true;
    };

    template<typename T>
    concept String = isString<T>::value;

    // helper declaration for static asserts
    template<typename... T>
    constexpr bool always_false = false;

    // Helper definitions for iterable concepts, assuming an object is iterable if it has begin() and end() functions
    // defined

    template<typename T>
    concept Iterable = requires(T x) {
                           x.begin();
                           x.end();
                       };

    template<typename T>
    concept Boolean = std::is_same_v<T, bool>;
}   // namespace fgc4::utils
