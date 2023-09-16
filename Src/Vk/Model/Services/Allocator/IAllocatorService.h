#pragma once

#include "../../Buffers/Buffer.h"

namespace VkCore
{
    class IAllocatorService
    {
      public:
        friend class Buffer;

        virtual ~IAllocatorService(){};

        /**
        * @brief takes data and info from the buffer and creates a new GPU buffer, allocates and populates it.
        * @param - A reference to an existing buffer. Used for obtaining info about the buffer.
        * @param - pointer to the data.
        */
        virtual void AllocateBufferOnGPU(Buffer& inoutBuffer, const void* data) = 0;
        virtual void DestroyBuffer(Buffer& buffer) = 0;
    };

} // namespace VkCore
