#include "NullAllocatorService.h"
#include "../../../../Log/Log.h"
#include "vulkan/vulkan_core.h"
#include <stdexcept>

namespace VkCore
{

    VkBuffer NullAllocatorService::CreateBuffer(const Buffer::BufferInfo& bufferInfo, VmaAllocation& outAllocation,
                                                VmaAllocationInfo* outAllocationInfo)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")

        return VK_NULL_HANDLE;
    }

    void NullAllocatorService::DestroyBuffer(Buffer& buffer)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

    void NullAllocatorService::CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const size_t size,
                                          const uint32_t srcOffset, const uint32_t dstOffset)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

    VkBuffer NullAllocatorService::CreateBufferOnGpu(const void* data, const Buffer::BufferInfo bufferInfo,
                                                     VmaAllocation& allocation, VmaAllocationInfo* allocationInfo)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")

        return VK_NULL_HANDLE;
    }

} // namespace VkCore
