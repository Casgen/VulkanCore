#pragma once

#include "IAllocatorService.h"

namespace VkCore
{

    class NullAllocatorService : public IAllocatorService
    {
      public:
        void AllocateBufferOnGPU(Buffer& inoutBuffer, const void* data) override;
        void DestroyBuffer(Buffer& buffer) override;

    };

} // namespace VkCore
