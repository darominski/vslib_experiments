#include "Util_Mmap.h"
#include "vslib_shared_memory_memmap.hpp"

namespace fgcd
{

    class Fgc4Shmem
    {
      public:
        Fgc4Shmem()
            : m_dev_mem("/dev/mem"),
              m_shared_memory(
                  m_dev_mem, app_data_0_1_ADDRESS, app_data_0_1_SIZE, PROT_READ | PROT_WRITE,
                  (void*)app_data_0_1_ADDRESS
              )
        {
        }

      private:
        File m_dev_mem;
        Mmap m_shared_memory;
    };

}   // namespace fgcd