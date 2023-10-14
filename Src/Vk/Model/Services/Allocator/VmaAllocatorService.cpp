#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>

#include <stdexcept>
#include "vulkan/vulkan.hpp"

#include "VmaAllocatorService.h"
#include "../../../Utils.h"
#include "../../../../Log/Log.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"

// Include it always all the way down!
#include <string>
#include <vk_mem_alloc.h>

namespace VkCore
{
    VmaAllocatorService::VmaAllocatorService(Device& device, PhysicalDevice& physicalDevice, vk::Instance& instance)
        : m_Device(device), m_PhysicalDevice(physicalDevice)
    {
        VmaAllocatorCreateInfo createInfo;

        // TODO: Refine the API Version definition
        createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        createInfo.device = *device;
        createInfo.physicalDevice = *physicalDevice;
        createInfo.instance = instance;
        createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        createInfo.pHeapSizeLimit = nullptr;
        createInfo.pAllocationCallbacks = nullptr;
        createInfo.pVulkanFunctions = nullptr;
        createInfo.pDeviceMemoryCallbacks = nullptr;
        createInfo.pTypeExternalMemoryHandleTypes = nullptr;

        vmaCreateAllocator(&createInfo, &m_VmaAllocator);
    }

    VmaAllocatorService::~VmaAllocatorService()
    {
        vmaDestroyAllocator(m_VmaAllocator);
    }

    void VmaAllocatorService::DestroyBuffer(Buffer& buffer)
    {
        vmaDestroyBuffer(m_VmaAllocator, buffer.GetVkBuffer(), buffer.GetVmaAllocation());
    }

    void VmaAllocatorService::CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const size_t size,
                                         const uint32_t srcOffset, const uint32_t dstOffset)
    {
        // Create a Transient pool only for this transfer command.
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
        poolInfo.queueFamilyIndex = m_PhysicalDevice.GetQueueFamilyIndices().m_GraphicsFamily.value();

        vk::CommandPool commandPool;
        vk::CommandBuffer commandBuffer;

        TRY_CATCH_BEGIN()

        commandPool = m_Device.CreateCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo{commandPool, vk::CommandBufferLevel::ePrimary, 1};

        commandBuffer = m_Device.AllocateCommandBuffers(allocInfo)[0];

        TRY_CATCH_END()

        // Start the command
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);

        {
            vk::BufferCopy copyRegion{srcOffset, dstOffset, size};

            commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
        }

        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(commandBuffer);

        m_Device.GetGraphicsQueue().submit(submitInfo);
        m_Device.GetGraphicsQueue().waitIdle();

        // Don't forget to free the Command buffer after it is done copying!
        m_Device.FreeCommandBuffer(commandPool, commandBuffer);
    }

    VkBuffer VmaAllocatorService::CreateBuffer(const Buffer::BufferInfo& bufferInfo, VmaAllocation& outAllocation,
                                               VmaAllocationInfo* outAllocationInfo)
    {
        if (bufferInfo.m_Size <= 0)
        {
            LOGF(Vulkan, Fatal,
                 "Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)! Given size was %d",
                 bufferInfo.m_Size)
            throw std::runtime_error("Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)!");
        }

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.size = bufferInfo.m_Size;

        VkBufferUsageFlags usageFlags = static_cast<VkBufferUsageFlags>(bufferInfo.m_UsageFlags);

        bufferCreateInfo.flags = static_cast<VkBufferCreateFlags>(bufferInfo.m_CreateFlags);
        bufferCreateInfo.usage = usageFlags;

        if (bufferInfo.m_QueueFamilyIndices.empty())
        {
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferCreateInfo.pQueueFamilyIndices = bufferInfo.m_QueueFamilyIndices.data();
            bufferCreateInfo.queueFamilyIndexCount = bufferInfo.m_QueueFamilyIndices.size();
        }
        else
        {
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
        }

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.pool = nullptr;
        allocCreateInfo.usage = bufferInfo.m_MemoryUsage;
        allocCreateInfo.flags = bufferInfo.m_AllocCreateFlags;
        allocCreateInfo.priority = 1.f;

        VkBuffer handle = VK_NULL_HANDLE;
        outAllocation = VK_NULL_HANDLE;

        VkResult result;

        TRY_CATCH_BEGIN()

        result = vmaCreateBuffer(m_VmaAllocator, &bufferCreateInfo, &allocCreateInfo, &handle, &outAllocation,
                                 outAllocationInfo);

        TRY_CATCH_END()

        Utils::CheckVkResult(result);

        return handle;
    }

    VkBuffer VmaAllocatorService::CreateBufferOnGpu(const void* data, const Buffer::BufferInfo bufferInfo,
                                                    VmaAllocation& allocation, VmaAllocationInfo* allocationInfo)
    {

        if (data == nullptr)
        {
            const char* errorMsg = "Couldn't allocate buffer on the GPU! Pointer to the data is nullptr!";
            LOG(Vulkan, Fatal, errorMsg)
            throw std::runtime_error(errorMsg);
        }

        if (bufferInfo.m_Size <= 0)
        {
            LOGF(Vulkan, Fatal,
                 "Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)! Given size was %d",
                 bufferInfo.m_Size)
            throw std::runtime_error("Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)!");
        }

        // First create a staging Buffer to act as a CPU visible buffer.

        Buffer::BufferInfo stagingBufferInfo{};

        stagingBufferInfo.m_Size = bufferInfo.m_Size;
        stagingBufferInfo.m_UsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferInfo.m_MemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        stagingBufferInfo.m_AllocCreateFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                               VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                                               VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VmaAllocation stagingAllocation;
        VmaAllocationInfo stagingAllocationInfo;
        VkBuffer stagingBuffer = CreateBuffer(stagingBufferInfo, stagingAllocation, &stagingAllocationInfo);

        // Copy the data from the memory to the staging buffer.
        std::memcpy(stagingAllocationInfo.pMappedData, data, bufferInfo.m_Size);

        // Create the GPU Buffer.
        //
        Buffer::BufferInfo gpuBufferInfo{};

        gpuBufferInfo.m_Size = bufferInfo.m_Size;
        gpuBufferInfo.m_UsageFlags = bufferInfo.m_UsageFlags | vk::BufferUsageFlagBits::eTransferDst;
        gpuBufferInfo.m_MemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        VkBuffer gpuBuffer = CreateBuffer(gpuBufferInfo, allocation, allocationInfo);

        if (!gpuBuffer)
        {
            const char* errorMsg = "Failed to create a Destination buffer!";
            LOG(Vulkan, Fatal, errorMsg);
            throw std::runtime_error(errorMsg);
        }

        VkMemoryPropertyFlags memPropFlags;
        vmaGetAllocationMemoryProperties(m_VmaAllocator, allocation, &memPropFlags);

        if (!(memPropFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
        {
            const char* errorMsg = "Failed to create a Destination Buffer! Buffer is not Device local!";
            LOG(Vulkan, Fatal, errorMsg);
            throw std::runtime_error(errorMsg);
        }

        // Copy the contents of the staging buffer to the gpu buffer.
        CopyBuffer(stagingBuffer, gpuBuffer, bufferInfo.m_Size);

        vmaDestroyBuffer(m_VmaAllocator, stagingBuffer, stagingAllocation);

        LOGF(Allocation, Verbose,
             "Buffer has been succesfully allocated and data has been transferred onto the GPU. Size: %d",
             bufferInfo.m_Size)

        return gpuBuffer;
    }

} // namespace VkCore
