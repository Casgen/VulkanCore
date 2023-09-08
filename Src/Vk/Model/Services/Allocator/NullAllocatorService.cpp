#include "NullAllocatorService.h"
#include "../../../../Log/Log.h"
#include <stdexcept>

namespace VkCore
{

    void NullAllocatorService::AllocateBufferOnGPU(Buffer& buffer)
    {
        LOG(Allocation, Fatal, "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }
} // namespace VkCore
