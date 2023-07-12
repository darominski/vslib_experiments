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
              m_p(p),
              m_i(i),
              m_d(d)
        {
            this->registerObject();
        };

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

        [[nodiscard]] const double* getAddressP() const
        {
            return m_p.address();
        }
        [[nodiscard]] const double* getAddressI() const
        {
            return m_i.address();
        }
        [[nodiscard]] const double* getAddressD() const
        {
            return m_d.address();
        }

      private:
        const std::string m_name;
        Param<double>     m_p{0.0};
        Param<double>     m_i{0.0};
        Param<double>     m_d{0.0};

        void registerObject();
    };

    void PID::registerObject()
    {
        addressRegistry::AddressRegistry& addrRegistry = addressRegistry::AddressRegistry::instance();
        addrRegistry.addToRegistry(
            this->m_name + ".p", reinterpret_cast<intptr_t>(this->getAddressP()), addressRegistry::TYPE::Float32
        );
        addrRegistry.addToRegistry(
            this->m_name + ".i", reinterpret_cast<intptr_t>(this->getAddressI()), addressRegistry::TYPE::Float32
        );
        addrRegistry.addToRegistry(
            this->m_name + ".d", reinterpret_cast<intptr_t>(this->getAddressD()), addressRegistry::TYPE::Float32
        );
    }
}   // PID namespace