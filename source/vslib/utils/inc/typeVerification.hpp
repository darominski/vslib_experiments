//! @file
//! @brief File containing utility functions for comparing JSON types with C++ standard types for use in Parameter
//! class.
//! @author Dominik Arominski

#include "staticJson.hpp"
#include "typeTraits.hpp"
#include "warningMessage.hpp"

namespace vslib::utils
{
    //! Validates the provided JSON scalar value against the templated unsigned integer Parameter type.
    //!
    //! @param value JSON object containing command value to be checked against the template type
    //! @tparam ParameterType Type of the Parameter to verify the JSON value against
    //! @return If validation not successful returns Warning with relevant information, nothing otherwise
    template<typename ParameterType>
    std::optional<fgc4::utils::Warning> checkIfUnsigned(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::Numeric<ParameterType>
    {
        // C++ standard does not differentiate between integral and boolean type in std::is_integral and
        // std::is_unsigned. Therefore, boolean has to be explicitely removed from comparison below.
        bool passed = true;
        if constexpr ((std::is_unsigned<ParameterType>::value && !std::is_same_v<ParameterType, bool>))
        {   // checks scalar numeric type
            passed = value.is_number_unsigned();
        }
        // else: T is not unsigned so there is no value in checking the JSON

        if (!passed)
        {
            fgc4::utils::Warning message(fmt::format(
                "The provided command value: {} is not an unsigned integer, while Parameter type is an unsigned "
                "integer.\n",
                value.dump()
            ));
            return message;
        }
        return {};
    }

    //! Validates the provided JSON array value against the templated unsigned integer-holding array Parameter type.
    //!
    //! @param value JSON object containing command value to be checked against the template type
    //! @tparam ParameterType Type of the Parameter to verify the JSON value against
    //! @return If validation not successful returns Warning with relevant information, nothing otherwise
    template<typename ParameterType>
    std::optional<fgc4::utils::Warning> checkIfUnsigned(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::NumericArray<ParameterType>
    {
        // C++ standard does not differentiate between integral and boolean type in std::is_integral and
        // std::is_unsigned. Therefore, boolean has to be explicitely removed from comparison below.
        if constexpr (std::is_unsigned<typename ParameterType::value_type>::value && !std::is_same_v<typename ParameterType::value_type, bool>)
        {   // checks internal type, e.g. stored in an array.
            if (value.is_array())
            {
                for (uint64_t index = 0; index < value.size(); index++)
                {
                    if (!value[index].is_number_unsigned())
                    {
                        fgc4::utils::Warning message(fmt::format(
                            "The provided command value: {} is not an array of unsigned integers with an issue at "
                            "index {}, while Parameter type is an array of unsigned integers.\n",
                            value.dump(), index
                        ));
                        return message;
                    }
                }
            }
            else
            {
                fgc4::utils::Warning message(fmt::format(
                    "The provided command value: {} is not an array, while Parameter type is an array of unsigned "
                    "integers.\n",
                    value.dump()
                ));
                return message;
            }
        }
        // else: T is not unsigned so there is no value in checking the JSON
        return {};
    }

    //! Validates the provided JSON scalar value against the templated boolean Parameter type.
    //!
    //! @param value JSON object containing command value to be checked against the template type
    //! @tparam ParameterType Type of the Parameter to verify the JSON value against
    //! @return If validation not successful returns Warning with relevant information, nothing otherwise
    template<typename ParameterType>
    std::optional<fgc4::utils::Warning> checkIfBoolean(const fgc4::utils::StaticJson& value)
    {
        bool passed = true;
        if (fgc4::utils::Boolean<ParameterType>)
        {
            passed = value.is_boolean();
        }
        // else: T is not a boolean nor boolean stored by an array

        if (!passed)
        {
            fgc4::utils::Warning message(fmt::format(
                "The provided command value: {} is not an unsigned integer, while Parameter type is an unsigned "
                "integer.\n",
                value.dump()
            ));
            return message;
        }
        return {};
    }

    //! Validates the provided JSON scalar value against the templated boolean-holding array Parameter type.
    //!
    //! @param value JSON object containing command value to be checked against the template type
    //! @tparam ParameterType Type of the Parameter to verify the JSON value against
    //! @return If validation not successful returns Warning with relevant information, nothing otherwise
    template<typename ParameterType>
    std::optional<fgc4::utils::Warning> checkIfBoolean(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::StdArray<ParameterType>
    {
        if constexpr (fgc4::utils::Boolean<typename ParameterType::value_type>)
        {
            if (value.is_array())
            {
                for (uint64_t index = 0; index < value.size(); index++)
                {
                    if (!value[index].is_boolean())
                    {
                        fgc4::utils::Warning message(fmt::format(
                            "The provided command value: {} is not an array of bool with an issue at index {}, while "
                            "Parameter type is an array of bool.\n",
                            value.dump(), index
                        ));
                        return message;
                    }
                }
            }
            else
            {
                fgc4::utils::Warning message(fmt::format(
                    "The provided command value: {} is not an array, while Parameter type is an array of bool.\n",
                    value.dump()
                ));
                return message;
            }
        }
        // else: T is not a boolean nor boolean stored by an array
        return {};
    }

    //! Validates the provided JSON scalar value against the templated integral Parameter type.
    //!
    //! @param value JSON object containing command value to be checked against the template type
    //! @tparam ParameterType Type of the Parameter to verify the JSON value against
    //! @return If validation not successful returns Warning with relevant information, nothing otherwise
    template<typename ParameterType>
    std::optional<fgc4::utils::Warning> checkIfIntegral(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::Numeric<ParameterType>
    {
        // The boolean type has to be explicitly excluded from the integral type
        bool passed = true;
        if constexpr (std::is_integral<ParameterType>() && !std::is_same_v<ParameterType, bool>)
        {
            passed = value.is_number_integer();
        }

        if (!passed)
        {
            fgc4::utils::Warning message(fmt::format(
                "The provided command value: {} is not an integer, while Parameter type is an integer.\n", value.dump()
            ));
            return message;
        }
        return {};
    }

    //! Validates the provided JSON scalar value against the templated integral-holding array Parameter type.
    //!
    //! @param value JSON object containing command value to be checked against the template type
    //! @tparam ParameterType Type of the Parameter to verify the JSON value against
    //! @return If validation not successful returns Warning with relevant information, nothing otherwise
    template<typename ParameterType>
    std::optional<fgc4::utils::Warning> checkIfIntegral(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::NumericArray<ParameterType>
    {
        // The boolean type has to be explicitly excluded from the integral type
        if constexpr (std::is_integral<typename ParameterType::value_type>() && !std::is_same_v<typename ParameterType::value_type, bool>)
        {
            if (value.is_array())
            {
                for (uint64_t index = 0; index < value.size(); index++)
                {
                    if (!value[index].is_number_integer())
                    {
                        fgc4::utils::Warning message(fmt::format(
                            "The provided command value: {} is not an array of integers with an issue at index {}, "
                            "while Parameter type is an array of integers.\n",
                            value.dump(), index
                        ));
                        return message;
                    }
                }
            }
            else
            {
                fgc4::utils::Warning message(fmt::format(
                    "The provided command value: {} is not an array, while Parameter type is an array of integers.\n",
                    value.dump()
                ));
                return message;
            }
        }
        return {};
    }

    // other comparisons are not necessary: e.g. int to floating point should be lossless except for extremely large
    // numbers

}   // namespace vslib::utils