//! @file
//! @brief Source file containing library-side background task specific declarations for creating and uploading the
//! parameter map, validation of incoming commands, executing them, and triggering synchronisation of buffers.
//! @author Dominik Arominski

#pragma once

#include "sharedMemory.h"
#include "staticJson.h"

namespace vslib
{
    class BackgroundTask
    {
      public:
        BackgroundTask(SharedMemory& shared_memory)
            : m_shared_memory_ref(shared_memory)
        {
        }

        void initializeMemory();
        void uploadParameterMap();
        void receiveJsonCommand();
        void executeJsonCommand(const fgc4::utils::StaticJson&);
        void processJsonCommands(const fgc4::utils::StaticJson&);
        void synchroniseReadBuffers();
        bool validateJsonCommand(const fgc4::utils::StaticJson&);

      private:
        bool          m_received_new_data{false};
        SharedMemory& m_shared_memory_ref;
    };

}   // namespace vslib::backgroundTask
