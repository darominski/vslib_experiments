//! @file
//! @brief File containing utility functions for comparing JSON types with C++ standard types for use in Parameter
//! class.
//! @author Dominik Arominski

#include "staticJson.h"
#include "typeTraits.h"

namespace vslib::utils
{
    template<typename T>
    bool checkIfUnsigned(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::Numeric<T>
    {
        // C++ standard does not differentiate between integral and boolean type in std::is_integral and
        // std::is_unsigned. Therefore, boolean has to be explicitely removed from comparison below.
        bool passed = true;
        if constexpr ((std::is_unsigned<T>::value && !std::is_same_v<T, bool>))
        {   // checks scalar numeric type
            passed = value.is_number_unsigned();
        }
        // else: T is not unsigned so there is no value in checking the JSON
        return passed;
    }

    template<typename T>
    bool checkIfUnsigned(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::NumericArray<T>
    {
        // C++ standard does not differentiate between integral and boolean type in std::is_integral and
        // std::is_unsigned. Therefore, boolean has to be explicitely removed from comparison below.
        bool passed = true;
        if constexpr (std::is_unsigned<typename T::value_type>::value && !std::is_same_v<typename T::value_type, bool>)
        {   // checks internal type, e.g. stored in an array.
            if (value.is_array())
            {
                passed = value[0].is_number_unsigned();   // verify the first element of the array
            }
            else
            {
                passed = false;   // provided value is not an array, so it cannot be an array of unsigned integers
            }
        }
        // else: T is not unsigned so there is no value in checking the JSON
        return passed;
    }

    template<typename T>
    bool checkIfBoolean(const fgc4::utils::StaticJson& value)
    {
        bool passed = true;
        if (fgc4::utils::Boolean<T>)
        {
            passed = value.is_boolean();
        }
        // else: T is not a boolean nor boolean stored by an array
        return passed;
    }

    template<typename T>
    bool checkIfBoolean(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::StdArray<T>
    {
        bool passed = true;
        if constexpr (fgc4::utils::Boolean<typename T::value_type>)
        {
            if (value.is_array())
            {
                passed = value.is_boolean();
            }
            else
            {
                passed = false;
            }
        }
        // else: T is not a boolean nor boolean stored by an array
        return passed;
    }

    template<typename T>
    bool checkIfIntegral(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::Numeric<T>
    {
        // The boolean type has to be explicitly excluded from the integral type
        bool passed = true;
        if constexpr (std::is_integral<T>() && !std::is_same_v<T, bool>)
        {
            passed = value.is_number_integer();
        }
        return passed;
    }

    template<typename T>
    bool checkIfIntegral(const fgc4::utils::StaticJson& value)
        requires fgc4::utils::NumericArray<T>
    {
        // The boolean type has to be explicitly excluded from the integral type
        bool passed = true;
        if constexpr (std::is_integral<typename T::value_type>() && !std::is_same_v<typename T::value_type, bool>)
        {
            if (value.is_array())
            {
                passed = value[0].is_number_integer();
            }
            else
            {
                passed = false;
            }
        }
        return passed;
    }

    // other comparisons are not necessary: e.g. int to floating point should be lossless except for extremely large
    // numbers

}   // namespace vslib::utils