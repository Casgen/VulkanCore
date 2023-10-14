
#include "../../Vendor/vma/vk_mem_alloc.h"
#include "Buffer.h"

#include "vulkan/vulkan_structs.hpp"
#include "../Services/ServiceLocator.h"
#include <alloca.h>
#include <cstdint>

namespace VkCore
{

    Buffer::~Buffer()
    {
        ServiceLocator::GetAllocatorService().DestroyBuffer(*this);
    }

    void Buffer::InitializeOnGpu(const void* data, const size_t size)
    {
        BufferInfo bufferInfo{};

        bufferInfo.m_UsageFlags = m_UsageFlags;
        bufferInfo.m_Size = size;

        m_Buffer =
            ServiceLocator::GetAllocatorService().CreateBufferOnGpu(data, bufferInfo, m_Allocation, &m_AllocationInfo);
    }

    void Buffer::InitializeOnCpu(const void* data, const size_t size, const bool isMappable)
    {
        BufferInfo bufferInfo{};

        bufferInfo.m_UsageFlags = m_UsageFlags;
        bufferInfo.m_Size = size;
        bufferInfo.m_AllocCreateFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        bufferInfo.m_MemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

        if (isMappable)
        {
            bufferInfo.m_AllocCreateFlags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }

        ServiceLocator::GetAllocatorService().CreateBuffer(bufferInfo, m_Allocation, &m_AllocationInfo);

        std::memcpy(m_AllocationInfo.pMappedData, data, size);
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

    void Buffer::SetVkBuffer(const VkBuffer& buffer)
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
    void Buffer::Destroy()
    {
        ServiceLocator::GetAllocatorService().DestroyBuffer(*this);
    }

} // namespace VkCore
