#pragma once

#include <array>
#include <string>

#include "addressRegistry.h"
#include "param.h"

namespace component
{
    class RST
    {
      public:
        RST()                      = delete;   // disallow users from creating anonymous RSTs
        RST(RST& other)            = delete;   // and cloning objects
        void operator=(const RST&) = delete;   // as well as assigning

        RST(const std::string& name, std::array<double, 4> r)
            : m_name(name),
              m_r(name + ".r", r)
        {
        }

        [[nodiscard]] const std::string& getName() const
        {
            return m_name;
        }
        [[nodiscard]] auto const& getR() const
        {
            return m_r.value();
        }

      private:
        const std::string                        m_name;
        parameters::Param<std::array<double, 4>> m_r;
    };
}   // component namespace