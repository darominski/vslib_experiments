#pragma once

#include <iostream>
#include <string>

#include "addressRegistry.h"

namespace Parameters
{

    template<typename T>
    class Param
    {
      public:
        Param(T value)
            : m_value(value)
        {
        }
        // cannot assign nor clone Params
        Param& param(Param&)           = delete;
        void   operator=(const Param&) = delete;

        [[nodiscard]] const T& value() const
        {
            return m_value;
        }
        [[nodiscard]] const T* address() const
        {
            return &m_value;
        }

      private:
        T m_value;
    };

}   // Parameters namespace
