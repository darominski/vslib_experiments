//! @file
//! @brief File defining templated self-registering parameters class and its interface to define externally-settable
//! component parameters.
//! @author Dominik Arominski

#pragma once

#include <compare>
#include <concepts>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "component.hpp"
#include "constants.hpp"
#include "errorCodes.hpp"
#include "errorMessage.hpp"
#include "iparameter.hpp"
#include "magic_enum.hpp"
#include "parameterRegistry.hpp"
#include "staticJson.hpp"
#include "typeLabel.hpp"
#include "typeVerification.hpp"
#include "warningMessage.hpp"

namespace vslib
{
    // ************************************************************
    // Convenience constants for parameter class
    constexpr uint16_t number_buffers = 2;   // number of buffers for settable parameters

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
        using StaticJson = fgc4::utils::StaticJson;

      public:
        //! Constructor for parameters of non-numeric types and thus with no limits.
        //!
        //! @param parent Component owning this Parameter
        //! @param name Name of the Parameter
        Parameter(Component& parent, std::string_view name) noexcept
            requires fgc4::utils::NonNumeric<T>
            : m_name{name},
              m_parent{parent}
        {
            m_parent.registerParameter(*this);
        }

        //! Constructor for parameters with optional numeric-type limits.
        //!
        //! @param parent Component owning this Parameter
        //! @param name Name of the Parameter
        //! @param limit_min Minimal numerical value that this Parameter can be set to
        //! @param limit_max Maximal numerical value that this Parameter can be set to
        Parameter(
            Component& parent, std::string_view name,
            LimitType<T> limit_min = std::numeric_limits<LimitType<T>>::lowest(),
            LimitType<T> limit_max = std::numeric_limits<LimitType<T>>::max()
        )
            requires fgc4::utils::Numeric<T>
            : m_name{name},
              m_parent{parent},
              m_limit_min{limit_min},
              m_limit_max{limit_max},
              m_limit_min_defined{limit_min != std::numeric_limits<LimitType<T>>::lowest()},
              m_limit_max_defined(limit_max != std::numeric_limits<LimitType<T>>::max())
        {
            m_parent.registerParameter(*this);
        };

        // ************************************************************
        // Operator overloads to seamless interactions with held values

        //! Provides the access to the value and performs an implicit conversion to the desired type
        //!
        //! @return Read-buffer value.
        operator T() const
        {
            return *m_read_buffer;
        }

        //! Provides element-access to the values stored in the value, provided the type stored is a std::array.
        //!
        //! @param index Index of the array to be accessed. If invalid, an out_of_range exception will be thrown
        //! @return Value stored at the requested index
        const auto& operator[](uint64_t index) const
            requires fgc4::utils::StdArray<T>
        {
            if (index >= std::tuple_size_v<T>)
            {
                fgc4::utils::Error message(
                    fmt::
                        format("Out of bounds access attempted at index: {}, array size: {}.\n", index, std::tuple_size_v<T>),
                    fgc4::utils::errorCodes::out_of_bounds_access
                );
                throw std::out_of_range(fmt::format("{}", message));
            }
            return (*m_read_buffer)[index];
        }

        //! Provides ordering for the Parameters, allowing to compare them to allow interactions as if they were of the
        //! stored type.
        //!
        //! @param other Right-hand side Parameter to compare this Parameter against
        //! @return Ordering between the current Parameter and the one we compared the object to
        auto operator<=>(const Parameter& other) const noexcept
        {
            // parameters are compared based on the values stored
            // in the currently active buffer
            auto const& lhs = *(this->m_read_buffer);
            auto const& rhs = *(other.m_read_buffer);
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
        // Various helper getters

        //! Explicit conversion value getter function. It removes issues with
        //! implicit conversion of a complex type, e.g. arrays, enums, etc.
        //!
        //! @return Value held in the read buffer cast explictly to the Parameter type
        [[nodiscard]] const T& value() const noexcept
        {
            return *m_read_buffer;
        }

        //! Returns the write buffer value to be validated.
        //!
        //! @return Write buffer value with explict cast to the underlying type
        [[nodiscard]] const T& toValidate() const noexcept
        {
            return *m_write_buffer;
        }

        //! Getter for the initialization flag of the Parameter.
        //!
        //! @return True if the Parameter has been initialized, false otherwise
        [[nodiscard]] bool isInitialized() const noexcept override
        {
            return m_initialized;
        }

        //! Getter for the validate flag of the Parameter.
        //!
        //! @return True if the Parameter has been validated, false otherwise
        [[nodiscard]] bool isValidated() const noexcept override
        {
            return m_validated;
        }

        //! Getter for the Parameter name.
        //!
        //! @return Parameter name
        [[nodiscard]] std::string_view getName() const noexcept override
        {
            return m_name;
        }

        //! Getter for whether the lower numerical limit is defined.
        //!
        //! @return True if the lower limit is defined, false otherwise
        [[nodiscard]] bool isLimitMinDefined() const noexcept
        {
            return m_limit_min_defined;
        }

        //! Getter for whether the upper numerical limit is defined.
        //!
        //! @return True if the upper limit is defined, false otherwise
        [[nodiscard]] bool isLimitMaxDefined() const noexcept
        {
            return m_limit_max_defined;
        }

        //! Getter for the lower limit of the held value.
        //!
        //! @return Lower limit of allowed stored value
        [[nodiscard]] const LimitType<T>& getLimitMin() const noexcept
            requires fgc4::utils::Numeric<T>
        {
            return m_limit_min;
        }

        //! Getter for the upper limit of the held value.
        //!
        //! @return Upper limit of allowed stored value
        [[nodiscard]] const LimitType<T>& getLimitMax() const noexcept
            requires fgc4::utils::Numeric<T>
        {
            return m_limit_max;
        }

        // ************************************************************
        // Methods used in case the type T is an STL container, so it requires a begin and end methods for iteration

        //! Provides connection to begin() method of underlying container, if the Parameter type is an array.
        //!
        //! @return Mutable access to the beginning of the stored std::array
        auto begin()
            requires fgc4::utils::StdArray<T>
        {
            return m_read_buffer->begin();
        }

        //! Provides connection to cbegin() method of underlying container, if the Parameter type is an array.
        //!
        //! @return Non-mutable access to the beginning of the stored std::array
        auto const cbegin() const
            requires fgc4::utils::StdArray<T>
        {
            return m_read_buffer->cbegin();
        }

        //! Provides connection to end() method of underlying container, if the Parameter type is an array.
        //!
        //! @return Mutable access to the end of the stored std::array
        auto end()
            requires fgc4::utils::StdArray<T>
        {
            return m_read_buffer->end();
        }

        //! Provides connection to cend() method of underlying container, if the Parameter type is an array.
        //!
        //! @return Non-mutable access to the end of the stored std::array
        auto const cend() const
            requires fgc4::utils::StdArray<T>
        {
            return m_read_buffer->cend();
        }

        // ************************************************************

        //! Serializes this Parameter using JSON serialization class.
        //!
        //! @param serializer Reference to ParameterSerializer visitor
        //! @return JSON-serialized Parameter
        [[nodiscard]] StaticJson serialize(const ParameterSerializer& serializer) const noexcept override
        {
            return serializer.serialize(*this);
        }

        // ************************************************************

        //! Sets the provided JSON-serialized value to the parameter-held value.
        //!
        //! @param json_value JSON-serialized value to be set
        //! @return Returns a Warning with relevant information if setting not successful, nothing otherwise
        std::optional<fgc4::utils::Warning> setJsonValue(const StaticJson& json_value) override
        {
            auto const& maybe_warning = setJsonValueImpl(json_value);
            if (!maybe_warning.has_value())
            {
                // set the initialized flag
                m_initialized = true;
            }
            return maybe_warning;
        }

        //! Sets the initialization flag to the chosen value.
        //!
        //! @param initialized New value of the initialized flag of this Parameter.
        void setInitialized(const bool initialized) noexcept override
        {
            m_initialized = initialized;
        }

        //! Sets the validation flag to the provided value by the owning Component.
        //!
        //! @param value New value for the validated flag.
        void setValidated(const bool value) noexcept override
        {
            m_validated = value;
        }

        // ************************************************************
        // Method for synchronizing buffers

        //! Copies contents of the write buffer to the read buffer to synchronise them.
        void syncWriteBuffer() override
        {
            *m_write_buffer = *m_read_buffer;
        }

        //! Swaps the buffer pointers.
        void swapBuffers() override
        {
            T* tmp         = m_write_buffer;
            m_write_buffer = m_read_buffer;
            m_read_buffer  = tmp;
        }

        // ************************************************************

      private:
        const std::string m_name;     //!< Name of this Parameter
        Component&        m_parent;   //!< Component owning this Parameter

        std::array<T, number_buffers> m_value{T{}, T{}};   //!< Double-buffered value stored by this Parameter

        T* m_read_buffer{&m_value[0]};    //!< Pointer to the read buffer
        T* m_write_buffer{&m_value[1]};   //!< Pointer to the write buffer

        LimitType<T> m_limit_min;          //!< Minimum numerical value that can be stored
        LimitType<T> m_limit_max;          //!< Maximal numerical value that can be stored
        bool m_limit_min_defined{false};   //!< Flag whether the minimum limit has been set, used in serialization
        bool m_limit_max_defined{false};   //!< Flag whether the maximum limit has been set, used in serialization

        bool m_initialized{false};   //!< Flag defining whether the Parameter has been initialized
        bool m_validated{false};     //!< Flag defining whether the Parameter has been initialized and validated

        // ************************************************************
        // Methods related to checking the numerical limits of the parameter during parameter setting

        //! Checks whether the provided array values fall within the numerical limits specified for this
        //! Parameter.
        //!
        //! @param value New parameter values to be checked
        //! @return Warning with relevant information if check not successful, nothing otherwise
        std::optional<fgc4::utils::Warning> checkLimits(T value) const noexcept
            requires fgc4::utils::NumericArray<T>
        {
            // check if all of the provided values fit in the limits
            for (auto const& element : value)
            {
                if (m_limit_min > element || element > m_limit_max)
                {
                    fgc4::utils::Warning message(fmt::format(
                        "Value in the provided array: {} is outside the limits: {}, {}.\n", element, m_limit_min,
                        m_limit_max
                    ));
                    return message;
                }
            }
            return {};
        }

        //! Checks limits of all arithmetic types, if they are defined.
        //!
        //! @param value New parameter value to be checked
        //! @return Warning with relevant information if check not successful, nothing otherwise
        std::optional<fgc4::utils::Warning> checkLimits(T value) const noexcept
            requires fgc4::utils::NumericScalar<T>
        {
            if (value < m_limit_min || value > m_limit_max)
            {
                fgc4::utils::Warning message(
                    fmt::format("Provided value: {} is outside the limits: {}, {}.\n", value, m_limit_min, m_limit_max)
                );
                return message;
            }
            return {};
        }

        //! Fallback of checking limits for cases not comparable directly with double type: strings,
        //! enumerations, non-numerical arrays, etc.
        //!
        //! @return Empty optional return (success)
        std::optional<fgc4::utils::Warning> checkLimits(T) const noexcept
        {
            return {};
        }

        // ************************************************************
        // Methods related to setting new parameter value based on the JSON input

        //! Validates the provided JSON value against the scalar Parameter type.
        //!
        //! @param json_value JSON object containing command value to be validated
        //! @return If validation not successful returns Warning with relevant information, nothing otherwise
        std::optional<fgc4::utils::Warning> verifyTypeAgrees(const StaticJson& json_value)
        {
            auto const integral_check = utils::checkIfIntegral<T>(json_value);
            if (integral_check.has_value())
            {
                return integral_check.value();
            }

            auto const unsigned_check = utils::checkIfUnsigned<T>(json_value);
            if (unsigned_check.has_value())
            {
                return unsigned_check.value();
            }

            auto const boolean_check = utils::checkIfBoolean<T>(json_value);
            if (boolean_check.has_value())
            {
                return boolean_check.value();
            }
            return {};
        }

        //! Sets the provided JSON value to the write buffer. Handles boolean, std::string, numerical types, and arrays.
        //!
        //! @param json_value JSON object containing new parameter value to be set
        //! @return If not successful returns Warning with relevant information, nothing otherwise
        std::optional<fgc4::utils::Warning> setJsonValueImpl(const StaticJson& json_value)
        {
            // Try to implicitly cast the json_value-stored value to the parameter's type
            // This step removes steps that are not implicitly castable, e.g. string and numeric types
            T command_value;
            try   // try to extract the value stored in json_value
            {
                command_value = json_value;   // implicit internal cast in nlohmann library
            }
            catch (nlohmann::json::exception& e)
            {
                fgc4::utils::Warning message(fmt::format("{}.\n", e.what()));
                return message;
            }

            // Command type is implicitly castable, this step ensures that numerical types agree more precisely:
            // integral vs floating point agreement and signedness are checked
            if constexpr (fgc4::utils::NumericScalar<T> || fgc4::utils::Boolean<T> || fgc4::utils::NumericArray<T>)
            {
                auto const warning = verifyTypeAgrees(json_value);
                if (warning.has_value())
                {
                    return warning.value();
                }
            }
            // When types and their details are in agreement, the command value needs to fit in the Parameter's (and the
            // type's) limits
            auto const check_status = checkLimits(command_value);
            if (check_status.has_value())
            {
                return check_status.value();
            }
            else   // no issues, value can be safely set
            {
                *m_write_buffer = command_value;
                return {};
            }
        }

        //! Sets the provided JSON value with enumeration element to the write buffer.
        //!
        //! @param json_value JSON object containing new parameter value to be set
        //! @return If not successful returns Warning with relevant information, nothing otherwise
        std::optional<fgc4::utils::Warning> setJsonValueImpl(const StaticJson& json_value)
            requires fgc4::utils::Enumeration<T>
        {
            // json_value is then a string, try to cast to that:
            auto const enum_element = magic_enum::enum_cast<T>(std::string(json_value));
            if (enum_element.has_value())
            {
                *m_write_buffer = enum_element.value();
            }
            else
            {
                fgc4::utils::Warning message(
                    fmt::format("The provided value: {} is not one of the allowed enum values.\n", json_value)
                );
                return message;
            }
            return {};
        }

        //! Sets the provided JSON value with an array of enumeration to the write buffer.
        //!
        //! @param json_value JSON object containing new parameter value to be set
        //! @return If not successful returns Warning with relevant information, nothing otherwise
        std::optional<fgc4::utils::Warning> setJsonValueImpl(const StaticJson& json_value)
            requires fgc4::utils::StdArray<T> && fgc4::utils::Enumeration<typename T::value_type>
        {
            // json_value is an array of strings, try to cast every element to the enum type:
            int counter = 0;
            for (const auto& value : json_value)
            {
                auto const enum_element = magic_enum::enum_cast<typename T::value_type>(std::string(value));
                if (enum_element.has_value())
                {
                    (*m_write_buffer)[counter] = enum_element.value();
                    counter++;
                }
                else
                {
                    fgc4::utils::Warning message(
                        fmt::format("The provided value: {} is not one of the allowed enum values.\n", value)
                    );
                    return message;
                }
            }
            return {};
        }
    };
}   // namespace vslib
