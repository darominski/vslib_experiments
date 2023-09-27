//! @file
//! @brief  Bumbleboot integration for FGC4
//! @author Martin Cejp

#pragma once

#include <fcntl.h>
#include <filesystem>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

namespace fgcd
{
    class File
    {
      public:
        File(char const* filename)
        {
            this->handle = open(filename, O_RDWR);
        }

        ~File()
        {
            close(this->handle);
        }

        int handle;
    };

    class Mmap
    {
      public:
        Mmap(File& file, off_t offset, size_t len, int prot, void* map_at = nullptr)
        {
            int flags = MAP_SHARED;
            if (map_at != nullptr)
            {
                flags |= MAP_FIXED_NOREPLACE;
            }
            this->base = mmap(map_at, len, prot, flags, file.handle, offset);
            if (this->base == nullptr)
            {
                perror("mmap");
                exit(1);
            }
            if (map_at != nullptr)
            {
                if (this->base != map_at)
                {
                    throw std::runtime_error("mmap wrong address");
                }
            }
        }

        uint32_t read32(size_t offset) const
        {
            return *reinterpret_cast<volatile uint32_t*>(reinterpret_cast<uint8_t*>(base) + offset);
        }

        void write32(size_t offset, uint32_t value) const
        {
            *reinterpret_cast<volatile uint32_t*>(reinterpret_cast<uint8_t*>(base) + offset) = value;
        }

        void* base;
    };

}
