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

#include "baseComponent.h"
#include "iparameter.h"
#include "logString.h"
#include "magic_enum/magic_enum.hpp"
#include "nlohmann/json.hpp"
#include "parameterRegistry.h"
#include "staticJson.h"
#include "typeLabel.h"

inline unsigned short buffer_switch = 0;

namespace vslib::parameters
{
    constexpr uint16_t number_buffers  = 3;                    // number of buffers for settable parameters
    constexpr uint16_t write_buffer_id = number_buffers - 1;   // write buffer is always the last one

    template<typename T>
    class Parameter : public IParameter
    {
      public:
        Parameter(
            component::BaseComponent& parent, std::string_view name, T value,
            double limit_min = -std::numeric_limits<double>::max(),
            double limit_max = std::numeric_limits<double>::max()
        )
            : IParameter(name),
              m_value{value, value, value},
              m_limit_min{limit_min},
              m_limit_max{limit_max}
        {
            parent.registerParameter(name, *this);
        };
        // cloning is forbidden
        Parameter& parameter(Parameter&)  = delete;
        // move construct is forbidden
        Parameter parameter(Parameter&&)  = delete;
        // copy-assign is forbidden
        void operator=(const Parameter&)  = delete;
        // move-assign is forbidden
        void operator=(const Parameter&&) = delete;

        ~Parameter() override = default;

        operator T() const
        {
            return m_value[buffer_switch];
        }

        auto& operator[](uint64_t index) const
            requires utils::StdArray<T>
        {
            return m_value[buffer_switch][index];
        }

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

        // Explicit conversion value getter function. It removes issues with
        // implicit conversion of a complex type, e.g. arrays, enums, etc.
        [[nodiscard]] const T& value() const
        {
            return m_value[buffer_switch];
        }

        // ************************************************************
        // Methods used in case the type T is an STL container, so it requires a begin and end methods for iteration

        //! Provides connection to begin() method of underlying container
        auto begin()
            requires utils::StdArray<T>
        {
            return m_value[buffer_switch].begin();
        }

        //! Provides connection to cbegin() method of underlying container
        auto const cbegin() const
            requires utils::StdArray<T>
        {
            return m_value[buffer_switch].cbegin();
        }

        //! Provides connection to end() method of underlying container
        auto end()
            requires utils::StdArray<T>
        {
            return m_value[buffer_switch].end();
        }

        //! Provides connection to cend() method of underlying container
        auto const cend() const
            requires utils::StdArray<T>
        {
            return m_value[buffer_switch].cend();
        }

        // ************************************************************
        // The code in this section could be a part of a serializing visitor

        //! Serializes the Parameter by exposing name, type, and in case of enumerations, all options
        //! applicable to this Parameter type. Calls serializeImpl to handle different types.
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serialize() const noexcept override
        {
            nlohmann::json serialized_parameter = {{"name", m_name}, {"type", utils::getTypeLabel<T>()}};
            serialized_parameter.merge_patch(serializeImpl());
            return serialized_parameter;
        }

        // ************************************************************
        // The code checking limits could be a part of a visitor class, provided the limits are exposed
        // to public interface.

        //! Checks whether the provided array values fall within the numerical limits specified for this
        //! Parameter.
        //!
        //! @param command_value New parameter values to be checked
        //! @return LogString with error information if check not successful, nothing otherwise
        std::optional<utils::LogString> checkLimits(T command_value) const noexcept
            requires std::equality_comparable_with<T, double> && utils::StdArray<T>
        {
            // check if all of the provided values fit in the limits
            auto const found = std::find_if(
                std::cbegin(command_value), std::cend(command_value),
                [&](const auto& element)
                {
                    return (element < m_limit_min || element > m_limit_max);
                }
            );
            if (found != std::end(command_value))
            {
                utils::LogString error_msg("At least one value in command value array is outside the limits!\n");
                std::cerr << error_msg;
                return error_msg;
            }
            return {};
        }

        //! Checks limits of all types comparable with double
        //!
        //! @param command_value New parameter value to be checked
        //! @return LogString with error information if check not successful, nothing otherwise
        std::optional<utils::LogString> checkLimits(T command_value) const noexcept
            requires std::equality_comparable_with<T, double>
        {
            if (command_value < m_limit_min || command_value > m_limit_max)
            {
                utils::LogString error_msg("Command value: ");
                // TODO: This does not cast correctly, try to fix it with proper fmt formatting
                error_msg += (char)command_value;
                error_msg += " outside the limits!\n";
                return error_msg;
            }
            return {};
        }

        //! Fallback of checking limits for cases not comparable directly with double type: strings,
        //! enumerations, non-numerical arrays, etc.
        //!
        //! @return Empty optional return (success)
        std::optional<utils::LogString> checkLimits(T) const noexcept
        {
            return {};
        }

        // ************************************************************
        // The code setting new values coming from StaticJson may be really tricky to move to a visitor,
        // as it would require accessing private data members of Parameter class, unless that visitor
        // was a friend.

        //! Sets the provided JSON-serialized value to the parameter-held value.
        //!
        //! @param json_value JSON-serialized value to be set
        //! @return If not successful returns LogString with error information, nothing otherwise
        std::optional<utils::LogString> setJsonValue(const utils::StaticJson& json_value) override
        {
            return setJsonValueImpl(json_value);
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
        double                        m_limit_min{-std::numeric_limits<double>::max()};
        double                        m_limit_max{std::numeric_limits<double>::max()};

        //! Serializes enumerations by providing number of objects of the type ('length') and enumeration values
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
            requires utils::Enumeration<T>
        {
            return {{"length", magic_enum::enum_count<T>()}, {"values", magic_enum::enum_names<T>()}};
        }

        //! Serializes std::array type by exposing the length of the array
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
            requires utils::StdArray<T>
        {
            return {{"length", std::tuple_size_v<T>}};
        }

        //! Serializes numeric types: integers and floating point numbers
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
            requires utils::NumericType<T>
        {
            return {{"length", 1}};
        }

        //! Default overload for catching unsupported types
        [[nodiscard("Serialization output of parameter should not be discarded")]] nlohmann::json
        serializeImpl() const noexcept
        {
            static_assert(utils::always_false<T>, "Type currently not serializable.");
            return {"length", 0};
        }


        //! Sets the provided JSON value to the write buffer.
        //!
        //! @param json_value JSON object containing new parameter value to be set
        //! @return If not successful returns LogString with error information, nothing otherwise
        std::optional<utils::LogString> setJsonValueImpl(const utils::StaticJson& json_value)
        {
            T command_value;
            try   // try to extract the value stored in json_value
            {
                command_value = json_value;   // implicit internal cast in nlohmann library
            }
            catch (nlohmann::json::exception& e)
            {
                utils::LogString error_msg("ERROR: ");
                error_msg += e.what();
                error_msg += "\n";
                error_msg += "Command ignored.\n";
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
        //! @return If not successful returns LogString with error information, nothing otherwise
        std::optional<utils::LogString> setJsonValueImpl(const utils::StaticJson& json_value)
            requires utils::Enumeration<T>
        {
            // json_value is then a string, try to cast to that:
            auto const enum_element = magic_enum::enum_cast<T>(std::string(json_value));
            if (enum_element.has_value())
            {
                m_value[write_buffer_id] = enum_element.value();
            }
            else
            {
                utils::LogString error_msg("ERROR: The provided enum value is not one of the allowed values!\n");
                error_msg += "Command ignored.\n";
                return error_msg;
            }
            return {};
        }
    };
}   // parameters namespace
