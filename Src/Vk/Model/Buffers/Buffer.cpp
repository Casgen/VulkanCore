
#include "../../Vendor/vma/vk_mem_alloc.h"
#include "Buffer.h"

#include "vulkan/vulkan_structs.hpp"
#include "../Services/ServiceLocator.h"

namespace VkCore
{

    Buffer::Buffer(void* data, const uint32_t size, const vk::BufferUsageFlags& usageFlags)
        : m_Size(size), m_UsageFlags(usageFlags), m_Data(data)
    {
        ServiceLocator::GetAllocatorService().AllocateBufferOnGPU(*this);
    }

    Buffer::~Buffer()
    {
    }
    void* Buffer::GetData()
    {
        return m_Data;
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

    void Buffer::SetVkBuffer(const vk::Buffer& buffer)
    {
        m_Buffer = buffer;
    }

    void Buffer::SetVmaAllocation(const VmaAllocation& allocation)
    {
        m_Allocation = allocation;
    }


} // namespace VkCore
