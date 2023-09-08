#pragma once

#include "IAllocatorService.h"

namespace VkCore
{

    class NullAllocatorService : public IAllocatorService
    {
      public:
        virtual void AllocateBufferOnGPU(Buffer& buffer);

    };

} // namespace VkCore
