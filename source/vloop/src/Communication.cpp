#include "Communication.h"

namespace mexico
{

    [[nodiscard]] const std::string& Communication::getMessage() const
    {
        return m_msg;
    }

    [[nodiscard]] int Communication::getProperty() const
    {
        return m_int_property;
    }

    void Communication::setProperty(int value)
    {
        m_int_property = value;
    }
}
