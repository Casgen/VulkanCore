#pragma once

#include "../../Buffers/Buffer.h"

namespace VkCore
{
    class IAllocatorService
    {
      public:
        friend class Buffer;

        virtual ~IAllocatorService(){};
        virtual void AllocateBufferOnGPU(Buffer& buffer) = 0;
    };

} // namespace VkCore
