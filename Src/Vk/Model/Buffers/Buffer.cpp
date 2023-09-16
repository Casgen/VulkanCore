
#include "../../Vendor/vma/vk_mem_alloc.h"
#include "Buffer.h"

#include "vulkan/vulkan_structs.hpp"
#include "../Services/ServiceLocator.h"
#include <alloca.h>
#include <cstdint>

namespace VkCore
{

    Buffer::Buffer(void* data, const size_t size, const vk::BufferUsageFlags& usageFlags)
        : m_Size(size), m_UsageFlags(usageFlags)
    {
        ServiceLocator::GetAllocatorService().AllocateBufferOnGPU(*this, data);
    }

    Buffer::~Buffer()
    {
        ServiceLocator::GetAllocatorService().DestroyBuffer(*this);
    }

    vk::BufferUsageFlags Buffer::GetUsageFlags() const
    {
        return m_UsageFlags;
    }

    uint32_t Buffer::GetSize() const
    {

        return m_Size;
    }

    vk::Buffer Buffer::GetVkBuffer() const
    {
        return m_Buffer;
    }

    VmaAllocation Buffer::GetVmaAllocation() const
    {
        return m_Allocation;
    }

    VmaAllocationInfo Buffer::GetVmaAllocationInfo() const
    {
        return m_AllocationInfo;
    }

    void Buffer::SetVkBuffer(const vk::Buffer& buffer)
    {
        m_Buffer = buffer;
    }

    void Buffer::SetVmaAllocation(const VmaAllocation& allocation)
    {
        m_Allocation = allocation;
    }

    void Buffer::SetVmaAllocationInfo(const VmaAllocationInfo& allocationInfo)
    {
        m_AllocationInfo = allocationInfo;
    }

    void Buffer::SetUsageFlags(const vk::BufferUsageFlags& flags)
    {
        m_UsageFlags = flags;
    }

} // namespace VkCore
