#pragma once

#include <array>
#include <string>
#include <variant>

#include "addressRegistry.h"

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
            return m_p;
        }
        [[nodiscard]] double getI() const
        {
            return m_i;
        }
        [[nodiscard]] double getD() const
        {
            return m_d;
        }

        [[nodiscard]] double* getAddressP()
        {
            return &m_p;
        }
        [[nodiscard]] double* getAddressI()
        {
            return &m_i;
        }
        [[nodiscard]] double* getAddressD()
        {
            return &m_d;
        }

      private:
        const std::string m_name{"__"};
        double            m_p{0.0};
        double            m_i{0.0};
        double            m_d{0.0};

        void registerObject();
    };

    void PID::registerObject()
    {
        if ((addressRegistry::registerCounter + 3) >= addressRegistry::addressRegistrySize)
        {
            addressRegistry::registerCounter = 0;   // start over and begin overwriting or raise a warning/error?
        }
        addressRegistry::addrRegistry[addressRegistry::registerCounter] = addressRegistry::AddressStruct(
            this->m_name + ".p", reinterpret_cast<intptr_t>(this->getAddressP()), addressRegistry::TYPE::Float32
        );
        addressRegistry::addrRegistry[addressRegistry::registerCounter + 1] = addressRegistry::AddressStruct(
            this->m_name + ".i", reinterpret_cast<intptr_t>(this->getAddressI()), addressRegistry::TYPE::Float32
        );
        addressRegistry::addrRegistry[addressRegistry::registerCounter + 2] = addressRegistry::AddressStruct(
            this->m_name + ".d", reinterpret_cast<intptr_t>(this->getAddressD()), addressRegistry::TYPE::Float32
        );
        addressRegistry::registerCounter += 3;
    }
}   // PID namespace