//! @file
//! @brief Simple class with three double-type parameters to simulate interactions with a future PID component.
//! @author Dominik Arominski

#pragma once

#include <string>

#include "addressRegistry.h"
#include "param.h"

namespace component
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

        double& p()
        {
            return m_p;
        }

        [[nodiscard]] auto const& getP() const
        {
            return m_p;
        }
        [[nodiscard]] auto const& getI() const
        {
            return m_i;
        }
        [[nodiscard]] auto const& getD() const
        {
            return m_d;
        }

      private:
        const std::string         m_name;
        parameters::Param<double> m_p;
        parameters::Param<double> m_i;
        parameters::Param<double> m_d;
    };
}   // component namespace