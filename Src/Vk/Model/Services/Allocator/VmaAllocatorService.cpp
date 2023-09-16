#include <cstdint>
#include <cstring>
#include <iterator>

#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "VmaAllocatorService.h"
#include "../../../Utils.h"
#include "../../../../Log/Log.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"

// Include it always all the way down!
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

    void VmaAllocatorService::AllocateBufferOnGPU(Buffer& buffer, const void* data)
    {

        if (data == nullptr) {
            LOG(Vulkan, Fatal, "Couldn't allocate buffer on the GPU! Pointer to the data is nullptr!")
            throw std::runtime_error("Couldn't allocate buffer on the GPU! Pointer to the data is nullptr!");
        }

        uint32_t dataSize = buffer.GetSize();

        // First create a staging Buffer to act as a CPU visible buffer.
        VmaAllocation srcAllocation;
        VkBuffer srcBuffer = CreateBuffer(dataSize, vk::BufferUsageFlagBits::eTransferSrc,
                                          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, srcAllocation);

        // Copy the data from the memory to the staging buffer.
        void* mapData = nullptr;

        vmaMapMemory(m_VmaAllocator, srcAllocation, &mapData);
        std::memcpy(mapData, data, dataSize);
        vmaUnmapMemory(m_VmaAllocator, srcAllocation);

        // Create the GPU Buffer.

        VmaAllocation dstAllocation;

        VkBuffer dstBuffer = CreateBuffer(dataSize, buffer.GetUsageFlags() | vk::BufferUsageFlagBits::eTransferDst,
                                          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, dstAllocation);

        // Copy the contents of the staging buffer to the gpu buffer.
        CopyBuffer(srcBuffer, dstBuffer, dataSize);

        vmaDestroyBuffer(m_VmaAllocator, srcBuffer, srcAllocation);

        buffer.SetVkBuffer(dstBuffer);
        buffer.SetVmaAllocation(dstAllocation);

        LOGF(Allocation, Verbose,
             "Buffer has been succesfully allocated and data has been transferred onto the GPU. Size: %d", dataSize)
    }

    void VmaAllocatorService::DestroyBuffer(Buffer& buffer)
    {
        vmaDestroyBuffer(m_VmaAllocator, buffer.GetVkBuffer(), buffer.GetVmaAllocation());
    }

    void VmaAllocatorService::CopyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const uint32_t size,
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

        vk::BufferCopy copyRegion{srcOffset, dstOffset, size};
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBufferCount(1).setPCommandBuffers(&commandBuffer);

        m_Device.GetGraphicsQueue().submit(submitInfo);
        m_Device.GetGraphicsQueue().waitIdle();

        // Don't forget to free the Command buffer after it is done copying!
        m_Device.FreeCommandBuffer(commandPool, commandBuffer);
    }

    VkBuffer VmaAllocatorService::CreateBuffer(const size_t size, vk::BufferUsageFlags usageFlags,
                                               VmaAllocationCreateFlags allocFlags, VmaAllocation& outAllocation,
                                               VmaAllocationInfo* outAllocationInfo, VmaMemoryUsage memoryUsage,
                                               vk::SharingMode sharingMode, vk::BufferCreateFlags bufferCreateFlags)
    {

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.size = size;
        bufferCreateInfo.sharingMode = static_cast<VkSharingMode>(sharingMode);
        bufferCreateInfo.flags = static_cast<VkBufferCreateFlags>(bufferCreateFlags);
        bufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(usageFlags);

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.pool = nullptr;
        allocCreateInfo.usage = memoryUsage;
        allocCreateInfo.flags = allocFlags;
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
} // namespace VkCore
