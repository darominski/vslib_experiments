#include <iostream>
#include <memory>

#include "Communication.h"

int main()
{
    auto mexComm = std::make_unique<mexico::Communication>();
    std::cout << (mexComm->getMessage()) << '\n';
    std::cout << "The unset property value is: " << (mexComm->getProperty()) << '\n';
    mexComm->setProperty();
    std::cout << "The set property value is: " << (mexComm->getProperty()) << '\n';
    return 0;
}
