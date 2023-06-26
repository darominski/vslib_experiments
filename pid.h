#pragma once

#include <array>
#include <string>
#include <variant>

// global constants defining variable sizes
constexpr short  maxNameLength       = 128;   // max length of component name, in characters
constexpr size_t addressRegisterSize = 100;   // max number of possible settings

enum TYPE
{
    Int32,
    Float32
};

struct AddressStruct
{
    AddressStruct(){};
    AddressStruct(const std::string& name, intptr_t addr, TYPE type)
        : m_addr(addr),
          m_type(type)
    {
        size_t length = name.size();
        length        = length < name.size() ? length : maxNameLength - 1;
        std::copy(name.begin(), name.begin() + length, m_name.begin());
        m_name[length] = '\0';
    };
    std::array<char, 128> m_name{};
    intptr_t              m_addr;
    TYPE                  m_type;
};

std::array<AddressStruct, addressRegisterSize> addressRegister;
static int                                     registerCounter = 0;

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
        if ((registerCounter + 3) >= addressRegisterSize)
        {
            registerCounter = 0;   // start over and begin overwriting or raise a warning/error?
        }
        addressRegister[registerCounter]
            = AddressStruct(this->m_name + ".p", reinterpret_cast<intptr_t>(this->getAddressP()), TYPE::Float32);
        addressRegister[registerCounter + 1]
            = AddressStruct(this->m_name + ".i", reinterpret_cast<intptr_t>(this->getAddressI()), TYPE::Float32);
        addressRegister[registerCounter + 2]
            = AddressStruct(this->m_name + ".d", reinterpret_cast<intptr_t>(this->getAddressD()), TYPE::Float32);
        registerCounter += 3;
    }
}   // PID namespace