#include "Buffer.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "../Services/ServiceLocator.h"
#include "../../Log/Log.h"
#include <alloca.h>
#include <cstdint>
#include <stdexcept>

namespace VkCore
{

    Buffer& Buffer::operator=(Buffer&& other)
    {
        if (this != &other)
        {
            m_Size = other.m_Size;
            other.m_Size = 0;

            m_UsageFlags = other.m_UsageFlags;
            m_IsHostVisible = other.m_IsHostVisible;
            m_IsMapped = other.m_IsMapped;

            m_Buffer = other.m_Buffer;
            other.m_Buffer = VK_NULL_HANDLE;

            m_Allocation = other.m_Allocation;
            other.m_Allocation = VK_NULL_HANDLE;

            m_AllocationInfo = other.m_AllocationInfo;
            other.m_AllocationInfo = {};
        }

        return *this;
    }

    Buffer::Buffer(Buffer&& other)
    {
        if (this != &other)
        {
            m_Size = other.m_Size;
            other.m_Size = 0;

            m_UsageFlags = other.m_UsageFlags;
            m_IsHostVisible = other.m_IsHostVisible;
            m_IsMapped = other.m_IsMapped;

            m_Buffer = other.m_Buffer;
            other.m_Buffer = VK_NULL_HANDLE;

            m_Allocation = other.m_Allocation;
            other.m_Allocation = VK_NULL_HANDLE;

            m_AllocationInfo = other.m_AllocationInfo;
            other.m_AllocationInfo = {};
        }
    }

    Buffer::~Buffer()
    {
        if (m_Buffer != VK_NULL_HANDLE)
        {
            ServiceLocator::GetAllocatorService().DestroyBuffer(*this);
        }
    }

    void Buffer::InitializeOnGpu(const void* data, const size_t size)
    {
        BufferInfo bufferInfo{};

        bufferInfo.m_UsageFlags = m_UsageFlags;
        bufferInfo.m_Size = size;

        m_Buffer =
            ServiceLocator::GetAllocatorService().CreateBufferOnGpu(data, bufferInfo, m_Allocation, &m_AllocationInfo);

        m_Size = size;
    }

    void Buffer::InitializeOnCpu(const void* data, const size_t size, const bool isMapped)
    {
        InitializeOnCpu(size, isMapped);
        UpdateData(data);
    }

    void Buffer::InitializeOnCpu(const size_t size, const bool isMapped)
    {
        BufferInfo bufferInfo{};

        bufferInfo.m_UsageFlags = m_UsageFlags;
        bufferInfo.m_Size = size;
        bufferInfo.m_AllocCreateFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        bufferInfo.m_MemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

        if (isMapped)
        {
            bufferInfo.m_AllocCreateFlags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }

        m_Buffer = ServiceLocator::GetAllocatorService().CreateBuffer(bufferInfo, m_Allocation, &m_AllocationInfo);

        if (isMapped && m_AllocationInfo.pMappedData == nullptr)
        {
            const char* errMsg = "Buffer was mapped, but the pMappedData is nullptr!";
            LOG(Vulkan, Fatal, errMsg)
            throw std::runtime_error(errMsg);
        }

        m_Size = size;
        m_IsMapped = isMapped;
        m_IsHostVisible = true;
    }

    void Buffer::UpdateData(const void* data)
    {
        UpdateData(data, m_Size);
    }

    void Buffer::UpdateData(const void* data, const size_t size)
    {

        if (size == 0 || data == nullptr)
        {
            LOG(Vulkan, Warning, "Data wasn't updated! It is either NULL or the size of the data is 0!")
            return;
        }

        if (m_IsMapped)
        {
            std::memcpy(m_AllocationInfo.pMappedData, data, size);
            return;
        }

        void* mappedPtr = nullptr;

        ServiceLocator::GetAllocatorService().MapMemory(m_Allocation, mappedPtr);
        std::memcpy(mappedPtr, data, size);
        ServiceLocator::GetAllocatorService().UnmapMemory(m_Allocation);
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
        m_Buffer = VK_NULL_HANDLE;
    }

} // namespace VkCore
