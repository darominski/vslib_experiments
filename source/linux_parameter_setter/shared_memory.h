#include "Util_Mmap.h"
#include "sharedMemoryVslib.h"

namespace fgcd
{

    class Fgc4Shmem
    {
      public:
        Fgc4Shmem()
            : m_dev_mem("/dev/mem"),
              m_shared_memory(
                  m_dev_mem, SHARED_MEMORY_ADDRESS, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE,
                  (void*)SHARED_MEMORY_ADDRESS
              )
        {
        }

      private:
        File m_dev_mem;
        Mmap m_shared_memory;
    };

}   // namespace fgcd