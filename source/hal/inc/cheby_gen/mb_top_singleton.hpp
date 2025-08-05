// This header file was created to wrap the automatically generated Top structure
// into a singleton to be created once and used multiple times throughout the HAL objects.
// Its lifetime does not need to be managed, as the Top structure contains only address
// shifts, and eventually could be made in a compilation-time object.

#pragma once

#include <cheby_gen/mb_top.hpp>

#include "ip_cores_memory_map.hpp"

namespace hal
{
    class Top
    {
      public:
        //! Provides an instance of the register shifts ipCores::Top class wrapped in a singleton
        //!
        //! @return Singular instance of the register shifts Top class
        static ipCores::Top& instance()
        {
            // Registry is constructed on first access
            static Top m_instance;
            return m_instance.m_registers;
        }

      private:
        Top()
            : m_registers(reinterpret_cast<uint8_t*>(fgc4::utils::constants::fpga_base_address))
        {
        }

        ipCores::Top m_registers;
    };

}
