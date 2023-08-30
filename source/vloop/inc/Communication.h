#pragma once

#include <string>

namespace mexico
{

    class Communication
    {
      public:
        const std::string& getMessage() const;
        int                getProperty() const;
        void               setProperty();

      private:
        std::string m_msg{"Hello Mexico!"};
        int         m_int_property{0};
    };
}
