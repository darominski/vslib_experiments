//! @file
//! @brief File defining templated self-registering parameters class and its interface to define externally-settable
//! component parameters.
//! @author Dominik Arominski

#pragma once

#include <compare>
#include <concepts>
#include <limits>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>

#include "component.h"
#include "constants.h"
#include "errorMessage.h"
#include "iparameter.h"
#include "json/json.hpp"
#include "magic_enum/magic_enum.hpp"
#include "parameterRegistry.h"
#include "staticJson.h"
#include "typeLabel.h"

inline unsigned short buffer_switch = 0;   // used to define which is the read buffer in use, values: 0 or 1

namespace vslib::parameters
{
    // ************************************************************
    // Convenience constants for parameter class
    constexpr uint16_t number_buffers  = 3;                    // number of buffers for settable parameters
    constexpr uint16_t write_buffer_id = number_buffers - 1;   // write buffer is always the last one

    // ************************************************************
    // Helper definitions to define the type for min/max limits for parameters

    template<typename T, bool IsArray>
    struct LimitTypeHelper
    {
        using type = T;
    };

    template<typename T>
    struct LimitTypeHelper<T, true>
    {
        using type = typename T::value_type;
    };

    template<typename T>
    using LimitType = typename LimitTypeHelper<T, fgc4::utils::StdArray<T>>::type;

    // ************************************************************

    template<typename T>
    class Parameter : public IParameter
    {
      public:
        //! Constructor for parameters without an initial default value and no limits.
        //! It will need to be initialized externally via JSON command.
        Parameter(components::Component& parent, std::string_view name) noexcept
            : IParameter(name)
        {
            parent.registerParameter(name, *this);
        }

        //! Constructor overload for non-numerical parameters with initial default value and no defined limits.
        Parameter(components::Component& parent, std::string_view name, T default_value)
            requires(!fgc4::utils::NumericType<T>)
            : IParameter(name),
              m_value{default_value, default_value, default_value},
              m_default_value{default_value}
        {
            parent.registerParameter(name, *this);
        };

        //! Constructor overload for parameters with initial default value and optionally numeric-type limits.
        Parameter(
            components::Component& parent, std::string_view name, T default_value,
            LimitType<T> limit_min = std::numeric_limits<LimitType<T>>::lowest(),
            LimitType<T> limit_max = std::numeric_limits<LimitType<T>>::max()
        )
            requires fgc4::utils::NumericType<T>
            : IParameter(name),
              m_value{default_value, default_value, default_value},
              m_default_value{default_value},
              m_limit_min{limit_min},
              m_limit_max{limit_max},
              m_limit_min_defined{limit_min != std::numeric_limits<LimitType<T>>::lowest()},
              m_limit_max_defined(limit_max != std::numeric_limits<LimitType<T>>::max())
        {
            auto const maybe_error = checkLimits(m_default_value);
            if (maybe_error.has_value())
            {
                // possible to inform the user that the program is ill-formed by specifying #error preprocessor
                // directive here, e.g.: #error
                std::cerr << fmt::format("{}", maybe_error.value());
            }
            // TODO: when FSM implemented, if error is returned, state should go to "CONFIGURATION_FAILURE"
            parent.registerParameter(name, *this);
        };

        //! Constructor overload for parameters with initial default value and optionally numeric-type limits
        //! if the type T is as std::array
        Parameter(
            components::Component& parent, std::string_view name, T default_value,
            LimitType<T> limit_min = std::numeric_limits<LimitType<T>>::lowest(),
            LimitType<T> limit_max = std::numeric_limits<LimitType<T>>::max()
        )
            requires fgc4::utils::StdArray<T>
            : IParameter(name),
              m_value{default_value, default_value, default_value},
              m_default_value{default_value},
              m_limit_min{limit_min},
              m_limit_max{limit_max},
              m_limit_min_defined{limit_min != std::numeric_limits<LimitType<T>>::lowest()},
              m_limit_max_defined(limit_max != std::numeric_limits<LimitType<T>>::max())
        {
            auto const maybe_error = checkLimits(m_default_value);
            if (maybe_error.has_value())
            {
                std::cerr << fmt::format("{}", maybe_error.value());
            }
            // TODO: when FSM implemented, if error is returned, state should go to "CONFIGURATION_FAILURE"
            parent.registerParameter(name, *this);
        };

        // copy construction is forbidden
        Parameter(Parameter&)             = delete;
        // move construction is forbidden
        Parameter(Parameter&&)            = delete;
        // copy-assign is forbidden
        void operator=(const Parameter&)  = delete;
        // move-assign is forbidden
        void operator=(const Parameter&&) = delete;

        ~Parameter() override = default;

        // ************************************************************
        // Operator overloads to seamless interactions with held values

        //! Provides the access to the value and performs an implicit conversion to the desired type
        operator T() const
        {
            return m_value[buffer_switch];
        }

        //! Provides element-access to the values stored in the value, provided the type stored is a std::array
        //!
        //! @return Value stored at the provided index
        auto& operator[](uint64_t index) const
            requires fgc4::utils::StdArray<T>
        {
            return m_value[buffer_switch][index];
        }

        //! Provides ordering for the Parameters, allowing to compare them to interact as if they were of the stored
        //! type
        //!
        //! @return Ordering between the current Parameter and the one we compared the object to
        auto operator<=>(const Parameter& other) const
        {
            // parameters are compared based on the values stored
            // in the currently active buffer
            auto const& lhs = this->m_value[buffer_switch];
            auto const& rhs = other.m_value[buffer_switch];
            if (lhs == rhs)
            {
                return std::partial_ordering::equivalent;
            }
            if (lhs > rhs)
            {
                return std::partial_ordering::greater;
            }
            if (lhs < rhs)
            {
                return std::partial_ordering::less;
            }
            // comparison reaches here if lhs or rhs are NaN or similar
            return std::partial_ordering::unordered;
        }

        // ************************************************************

        //! Explicit conversion value getter function. It removes issues with
        //! implicit conversion of a complex type, e.g. arrays, enums, etc.
        //!
        //! @return Value stored cast explictly to the underlying type
        [[nodiscard]] const T& value() const
        {
            return m_value[buffer_switch];
        }

        // ************************************************************
        // Methods used in case the type T is an STL container, so it requires a begin and end methods for iteration

        //! Provides connection to begin() method of underlying container
        //!
        //! @return Mutable access to the beginning of the stored std::array
        auto begin()
            requires fgc4::utils::StdArray<T>
        {
            return m_value[buffer_switch].begin();
        }

        //! Provides connection to cbegin() method of underlying container
        //!
        //! @return Non-mutable access to the beginning of the stored std::array
        auto const cbegin() const
            requires fgc4::utils::StdArray<T>
        {
            return m_value[buffer_switch].cbegin();
        }

        //! Provides connection to end() method of underlying container
        //!
        //! @return Mutable access to the end of the stored std::array
        auto end()
            requires fgc4::utils::StdArray<T>
        {
            return m_value[buffer_switch].end();
        }

        //! Provides connection to cend() method of underlying container
        //!
        //! @return Non-mutable access to the end of the stored std::array
        auto const cend() const
            requires fgc4::utils::StdArray<T>
        {
            return m_value[buffer_switch].cend();
        }

        // ************************************************************
        // The code in this section could be a part of a serializing visitor

        //! Serializes the Parameter by exposing name, type, and in case of enumerations, all options
        //! applicable to this Parameter type. Calls serializeImpl to handle different types.
        //!
        //! @return JSON object with fully-serialized parameter
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serialize() const noexcept override
        {
            // all parameters have a name and a type that can be fetched the same way
            nlohmann::json serialized_parameter = {{"name", m_name}, {"type", fgc4::utils::getTypeLabel<T>()}};
            // other type-dependent properties, e.g. size of the stored type needs to be handled individually
            serialized_parameter.merge_patch(serializeImpl());
            // minimum and maximum numerical limits can be also be serialized, if set
            if (m_limit_min_defined)
            {
                serialized_parameter["limit_min"] = m_limit_min;
            }
            if (m_limit_max_defined)
            {
                serialized_parameter["limit_max"] = m_limit_max;
            }
            return serialized_parameter;
        }

        // ************************************************************

        //! Checks whether the provided array values fall within the numerical limits specified for this
        //! Parameter.
        //!
        //! @param value New parameter values to be checked
        //! @return Error with error information if check not successful, nothing otherwise
        std::optional<fgc4::utils::Error> checkLimits(T value) const noexcept
            requires std::equality_comparable_with<T, double> && fgc4::utils::StdArray<T>
        {
            // check if all of the provided values fit in the limits
            for (auto const& element : value)
            {
                if (m_limit_min > element || element > m_limit_max)
                {
                    fgc4::utils::Error error_msg(
                        fmt::format(
                            "Value in the provided array: {} is outside the limits: {}, {}!\n", value, m_limit_min,
                            m_limit_max
                        ),
                        fgc4::utils::constants::error_json_command_value_outside_limits
                    );
                    std::cerr << fmt::format("{}", error_msg);
                    return error_msg;
                }
            }
            return {};
        }

        //! Checks limits of all arithmetic types, if they are defined
        //!
        //! @param value New parameter value to be checked
        //! @return Error with error information if check not successful, nothing otherwise
        std::optional<fgc4::utils::Error> checkLimits(T value) const noexcept
            requires std::is_arithmetic_v<T>
        {
            if (value < m_limit_min || value > m_limit_max)
            {
                fgc4::utils::Error error_msg(
                    fmt::format("Provided value: {} is outside the limits: {}, {}!\n", value, m_limit_min, m_limit_max),
                    fgc4::utils::constants::error_json_command_value_outside_limits
                );
                return error_msg;
            }
            return {};
        }

        //! Fallback of checking limits for cases not comparable directly with double type: strings,
        //! enumerations, non-numerical arrays, etc.
        //!
        //! @return Empty optional return (success)
        std::optional<fgc4::utils::Error> checkLimits(T) const noexcept
        {
            return {};
        }

        // ************************************************************

        //! Sets the provided JSON-serialized value to the parameter-held value.
        //!
        //! @param json_value JSON-serialized value to be set
        //! @return If not successful returns Error with error information, nothing otherwise
        std::optional<fgc4::utils::Error> setJsonValue(const fgc4::utils::StaticJson& json_value) override
        {
            auto const& maybe_error = setJsonValueImpl(json_value);
            if (!m_initialized && !maybe_error.has_value())
            {
                m_initialized = true;
            }
            return maybe_error;
        }

        // ************************************************************

        //! Copies all contents of a write buffer to the background buffer, which is not currently used.
        void synchroniseWriteBuffer() override
        {
            auto*      background_buffer = &m_value[buffer_switch ^ 1];
            auto*      write_buffer      = &m_value[write_buffer_id];
            auto const memory_size       = sizeof(T);
            // always copy from the write buffer to the background buffer
            memcpy(reinterpret_cast<void*>(background_buffer), reinterpret_cast<void*>(write_buffer), memory_size);
        }

        // ************************************************************

        //! Copies all contents of the currently used buffer to the background buffer to synchronise them.
        void synchroniseReadBuffers() override
        {
            auto*      active_buffer     = &m_value[buffer_switch];
            auto*      background_buffer = &m_value[buffer_switch ^ 1];
            auto const memory_size       = sizeof(T);
            // always copy from active buffer to the background buffer
            memcpy(reinterpret_cast<void*>(background_buffer), reinterpret_cast<void*>(active_buffer), memory_size);
        }

      private:
        std::array<T, number_buffers> m_value;
        T                             m_default_value;
        LimitType<T>                  m_limit_min;
        LimitType<T>                  m_limit_max;
        bool                          m_limit_min_defined{false};
        bool                          m_limit_max_defined{false};
        bool                          m_initialized{false};

        //! Serializes enumerations by providing number of objects of the type ('length') and enumeration values
        //!
        //! @return JSON object with information about the stored enumeration
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
            requires fgc4::utils::Enumeration<T>
        {
            return {
                {"length", magic_enum::enum_count<T>()},
                {"fields", magic_enum::enum_names<T>()},
                {"value", magic_enum::enum_name(m_value[buffer_switch])},
                {"default_value", magic_enum::enum_name(m_default_value)}};
        }

        //! Serializes std::array type by exposing the length of the array
        //!
        //! @return JSON object with information about the stored std::array
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
            requires fgc4::utils::StdArray<T>
        {
            return {
                {"length", std::tuple_size_v<T>},
                {"value", m_value[buffer_switch]},
                {"default_value", m_default_value}};
        }

        //! Serializes numeric types: integers and floating point numbers
        //!
        //! @return JSON object with informaton about the stored numerical values
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
            requires fgc4::utils::NumericType<T>
        {
            return {{"length", 1}, {"value", m_value[buffer_switch]}, {"default_value", m_default_value}};
        }

        //! Default overload for catching unsupported types. Blocks compilation for those types
        //!
        //! @return Empty JSON for unsupported type
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
        {
            static_assert(fgc4::utils::always_false<T>, "Type currently not serializable.");
            return {};
        }

        //! Sets the provided JSON value to the write buffer.
        //!
        //! @param json_value JSON object containing new parameter value to be set
        //! @return If not successful returns Error with error information, nothing otherwise
        std::optional<fgc4::utils::Error> setJsonValueImpl(const fgc4::utils::StaticJson& json_value)
        {
            T command_value;
            try   // try to extract the value stored in json_value
            {
                command_value = json_value;   // implicit internal cast in nlohmann library
            }
            catch (nlohmann::json::exception& e)
            {
                fgc4::utils::Error error_msg(
                    e.what() + std::string(".\nCommand ignored.\n"),
                    fgc4::utils::constants::error_json_command_value_type_invalid
                );
                return error_msg;
            }
            auto const check_status = checkLimits(command_value);
            if (check_status.has_value())
            {
                return check_status.value();
            }
            else
            {
                m_value[write_buffer_id] = command_value;
                return {};
            }
        }

        //! Sets the provided JSON value with enumeration element to the write buffer.
        //!
        //! @param json_value JSON object containing new parameter value to be set
        //! @return If not successful returns Error with error information, nothing otherwise
        std::optional<fgc4::utils::Error> setJsonValueImpl(const fgc4::utils::StaticJson& json_value)
            requires fgc4::utils::Enumeration<T>
        {
            // json_value is then a string, try to cast to that:
            auto const enum_element = magic_enum::enum_cast<T>(std::string(json_value));
            if (enum_element.has_value())
            {
                m_value[write_buffer_id] = enum_element.value();
            }
            else
            {
                fgc4::utils::Error error_msg(
                    "The provided enum value is not one of the allowed values.\nCommand ignored.\n",
                    fgc4::utils::constants::error_json_command_invalid_enum_value
                );
                return error_msg;
            }
            return {};
        }
    };
}   // parameters namespace
