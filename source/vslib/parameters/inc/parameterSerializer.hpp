//! @file
//! @brief File defining class used for serialization of the Parameter class.
//! @author Dominik Arominski

#pragma once

#include "magic_enum.hpp"
#include "staticJson.hpp"
#include "typeLabel.hpp"
#include "typeTraits.hpp"

namespace vslib
{

    using StaticJson = fgc4::utils::StaticJson;

    // forward declarations of serialized classes
    class IParameter;
    template<typename T>
    class Parameter;

    // ************************************************************

    class ParameterSerializer
    {
      public:
        //! Serializes the provided Parameter.
        //!
        //! @param parameter Parameter to be serialized
        //! @return JSON with serialized Parameter
        [[nodiscard]] StaticJson serialize(const IParameter& parameter) const noexcept
        {
            // all parameters have a name and a type that can be fetched the same way
            StaticJson serialized_parameter = {{"name", parameter.getName()}};
            // other type-dependent properties, e.g. length of the stored type needs to be handled individually
            serialized_parameter.merge_patch(parameter.serialize(*this));
            // minimum and maximum numerical limits can be also be serialized, if set
            return serialized_parameter;
        }

        // ************************************************************
        // Methods related to type-dependent serialization

        //! Serializes enumerations by providing number of objects of the type ('length') and enumeration values.
        //!
        //! @return JSON object with information about the stored enumeration
        template<typename T>
        [[nodiscard]] StaticJson serialize(const Parameter<T>& parameter) const noexcept
            requires fgc4::utils::Enumeration<T>
        {
            StaticJson serialized_parameter
                = {{"type", fgc4::utils::getTypeLabel<T>()},
                   {"length", magic_enum::enum_count<T>()},
                   {"fields", magic_enum::enum_names<T>()}};
            if (parameter.isInitialized())
            {
                serialized_parameter["value"] = magic_enum::enum_name(parameter.value());
            }
            else
            {
                serialized_parameter["value"] = nlohmann::json::object();
            }
            return serialized_parameter;
        }

        //! Serializes std::array type by exposing the length of the array, individual limits in case of a numeric
        //! type or available fields in case of an enumeration.
        //!
        //! @return JSON object with information about the stored std::array
        template<typename T>
        [[nodiscard]] StaticJson serialize(const Parameter<T>& parameter) const noexcept
            requires fgc4::utils::StdArray<T>
        {
            StaticJson serialized_parameter
                = {{"type", fgc4::utils::getTypeLabel<T>()}, {"length", std::tuple_size_v<T>}};
            if constexpr (fgc4::utils::NumericArray<T>)
            {
                if (parameter.isLimitMinDefined())
                {
                    serialized_parameter["limit_min"] = parameter.getLimitMin();
                }
                if (parameter.isLimitMaxDefined())
                {
                    serialized_parameter["limit_max"] = parameter.getLimitMax();
                }
            }
            else if constexpr (fgc4::utils::Enumeration<typename T::value_type>)
            {
                serialized_parameter["fields"] = magic_enum::enum_names<typename T::value_type>();
            }

            if (parameter.isInitialized())
            {
                if constexpr (fgc4::utils::Enumeration<typename T::value_type>)
                {
                    // special case for the enumerations, otherwise the array would be serialized as array of integers
                    // with index of the selected enumeration field rather than the name
                    serialized_parameter["value"] = nlohmann::json::array();
                    std::transform(
                        parameter.value().cbegin(), parameter.value().cend(),
                        std::back_inserter(serialized_parameter["value"]),
                        [](const auto& value)
                        {
                            return magic_enum::enum_name(value);
                        }
                    );
                }
                else
                {
                    serialized_parameter["value"] = parameter.value();
                }
            }
            else
            {
                serialized_parameter["value"] = nlohmann::json::array();
            }
            return serialized_parameter;
        }

        //! Serializes std::string type by exposing its length.
        //!
        //! @return JSON object with information about the stored std::array
        template<typename T>
        [[nodiscard]] StaticJson serialize(const Parameter<T>& parameter) const noexcept
            requires fgc4::utils::String<T>
        {
            StaticJson serialized_parameter
                = {{"type", fgc4::utils::getTypeLabel<T>()}, {"length", parameter.value().size()}};
            if (parameter.isInitialized())
            {
                serialized_parameter["value"] = parameter.value();
            }
            else
            {
                serialized_parameter["value"] = nlohmann::json::object();
            }
            return serialized_parameter;
        }

        //! Serializes numeric types: integers and floating point numbers.
        //!
        //! @return JSON object with informaton about the stored numerical values
        template<typename T>
        [[nodiscard]] StaticJson serialize(const Parameter<T>& parameter) const noexcept
            requires fgc4::utils::NumericScalar<T>
        {
            StaticJson serialized_parameter = {{"type", fgc4::utils::getTypeLabel<T>()}, {"length", 1}};
            if (parameter.isLimitMinDefined())
            {
                serialized_parameter["limit_min"] = parameter.getLimitMin();
            }
            if (parameter.isLimitMaxDefined())
            {
                serialized_parameter["limit_max"] = parameter.getLimitMax();
            }
            if (parameter.isInitialized())
            {
                serialized_parameter["value"] = parameter.value();
            }
            else
            {
                serialized_parameter["value"] = nlohmann::json::object();
            }
            return serialized_parameter;
        }

        //! Default overload for catching unsupported types. Blocks compilation for those types.
        //!
        //! @return Empty JSON for unsupported type
        template<typename T>
        [[nodiscard]] StaticJson serialize(const class Parameter<T>& parameter) const noexcept
        {
            static_assert(fgc4::utils::always_false<T>, "Type currently not serializable.");
            return {};
        }
    };
}   // namespace vslib