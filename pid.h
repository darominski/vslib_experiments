#pragma once

#include <array>
#include <string>
#include <variant>

#include "addressRegistry.h"
#include "param.h"

namespace PID
{
    class PID
    {
      public:
        PID()                      = delete;   // disallow users from creating anonymous PIDs
        PID(PID& other)            = delete;   // and cloning objects
        void operator=(const PID&) = delete;   // as well as assigning

        PID(const std::string& name, double p = 0.0, double i = 0.0, double d = 0.0)
            : m_name(name),
              m_p(name + ".p", p),
              m_i(name + ".i", i),
              m_d(name + ".d", d)
        {
        }

        [[nodiscard]] const std::string& getName() const
        {
            return m_name;
        }
        [[nodiscard]] double getP() const
        {
            return m_p.value();
        }
        [[nodiscard]] double getI() const
        {
            return m_i.value();
        }
        [[nodiscard]] double getD() const
        {
            return m_d.value();
        }

      private:
        const std::string         m_name;
        Parameters::Param<double> m_p;
        Parameters::Param<double> m_i;
        Parameters::Param<double> m_d;
    };
}   // PID namespace