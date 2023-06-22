#pragma once

#include <variant>
#include <vector>

enum TYPE {Int32, Float32};

struct AddressStruct {
    AddressStruct() {};
    AddressStruct(const std::string& name, std::variant<int*, double*> addr, TYPE type) : m_name(name), m_addr(addr), m_type(type) {};
    std::string m_name{""};
    std::variant<int*, double*> m_addr;
    TYPE m_type;
};

std::vector<AddressStruct> *addressRegister;
static int registerCounter = 0;

namespace PID {
class PID {
    public:
        PID() = delete; // disallow users from creating anonymous PIDs
        PID(const std::string &name, double p=0.0, double i=0.0, double d=0.0): m_name(name), m_p(p), m_i(i), m_d(d) {
            this->registerObject();
        };
        
        [[nodiscard]] const std::string& getName() const {return m_name; }
        [[nodiscard]] double getP() const {return m_p; }
        [[nodiscard]] double getI() const {return m_i; }
        [[nodiscard]] double getD() const {return m_d; }

        [[nodiscard]] double* getAddressP() {return &m_p; }
        [[nodiscard]] double* getAddressI() {return &m_i; }
        [[nodiscard]] double* getAddressD() {return &m_d; }

    private:
        const std::string m_name{"__"};
        double m_p{0.0};
        double m_i{0.0};
        double m_d{0.0};

        void registerObject();
};

void PID::registerObject() {
    std::cout << (*addressRegister).size() << std::endl;
    addressRegister[0][registerCounter] = AddressStruct(this->m_name + ".p", this->getAddressP(), TYPE::Float32);
    // addressRegister[0][registerCounter+1] = AddressStruct(this->m_name + ".i", this->getAddressI(), TYPE::Float32);
    // addressRegister[0][registerCounter+2] = AddressStruct(this->m_name + ".d", this->getAddressD(), TYPE::Float32);
    registerCounter += 3;
}
} // PID namespace