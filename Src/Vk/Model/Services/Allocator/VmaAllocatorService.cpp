#include <cstdint>
#include <cstring>
#include <iterator>

#include <vulkan/vulkan.hpp>

#include "VmaAllocatorService.h"
#include "../../../Utils.h"
#include "../../../../Log/Log.h"
#include "vulkan/vulkan_core.h"

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
        // createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

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

    void VmaAllocatorService::AllocateBufferOnGPU(Buffer& buffer)
    {
        // First create a staging Buffer to act as a CPU visible buffer.

        uint32_t dataSize = buffer.GetSize();

        // VkBufferCreateInfo srcBufferInfo;
        // srcBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        // srcBufferInfo.pNext = nullptr;
        // srcBufferInfo.size = dataSize;
        // srcBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        // srcBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // vk::BufferCreateInfo srcBufferInfo;
        // srcBufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        //     .setSharingMode(vk::SharingMode::eExclusive)
        //     .setSize(dataSize);
        //
        // VkBufferCreateInfo srcVkBufferInfo = srcBufferInfo;
        //
        // VmaAllocationCreateInfo stagingAllocInfo;
        // stagingAllocInfo.pool = nullptr;
        // stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        // stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        // stagingAllocInfo.priority = 1.f;
        //
        // VmaAllocation srcAllocation;

        // Utils::CheckVkResult(
        //     vmaCreateBuffer(m_VmaAllocator, &srcVkBufferInfo, &stagingAllocInfo, &srcBuffer, &srcAllocation,
        //     nullptr));

        VmaAllocation srcAllocation;
        VkBuffer srcBuffer = CreateBuffer(dataSize, buffer.GetUsageFlags() | vk::BufferUsageFlagBits::eTransferSrc,
                                          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, srcAllocation);

        // Copy the data from the memory to the staging buffer.
        void* mapData = nullptr;

        vmaMapMemory(m_VmaAllocator, srcAllocation, &mapData);
        std::memcpy(mapData, buffer.GetData(), dataSize);
        vmaUnmapMemory(m_VmaAllocator, srcAllocation);

        // Create the GPU Buffer.
        vk::BufferCreateInfo dstBufferInfo;
        dstBufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSize(dataSize);

        VkBufferCreateInfo dstVkBufferInfo = dstBufferInfo;

        VmaAllocationCreateInfo gpuAllocInfo;
        gpuAllocInfo.pool = nullptr;
        gpuAllocInfo.priority = 1.f;
        gpuAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        gpuAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer dstBuffer;
        VmaAllocation dstAllocation;

        Utils::CheckVkResult(
            vmaCreateBuffer(m_VmaAllocator, &dstVkBufferInfo, &gpuAllocInfo, &dstBuffer, &dstAllocation, nullptr));

        CopyBuffer(srcBuffer, dstBuffer, dataSize);

        vmaDestroyBuffer(m_VmaAllocator, srcBuffer, srcAllocation);

        buffer.SetVkBuffer(dstBuffer);
        buffer.SetVmaAllocation(dstAllocation);

        LOG(Allocation, Verbose, "Buffer has been allocated.")
    }

    void VmaAllocatorService::CopyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const uint32_t size,
                                         const uint32_t srcOffset, const uint32_t dstOffset)
    {
        // Copy the contents of the staging (src) buffer to the gpu (destination) buffer
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

        m_Device.FreeCommandBuffer(commandPool, commandBuffer);
    }
    VkBuffer VmaAllocatorService::CreateBuffer(const size_t size, vk::BufferUsageFlags usageFlags,
                                               VmaAllocationCreateFlags allocFlags, VmaAllocation& outAllocation,
                                               vk::SharingMode sharingMode, vk::BufferCreateFlags bufferCreateFlags,
                                               VmaMemoryUsage memoryUsage)
    {
        // vk::BufferCreateInfo srcBufferInfo;
        // srcBufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc).setSharingMode(sharingMode).setSize(size);

        VkBufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.size = size;
        // bufferCreateInfo.sharingMode = static_cast<VkSharingMode>(sharingMode);
        // bufferCreateInfo.flags = static_cast<VkBufferCreateFlags>(bufferCreateFlags);
        // bufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(usageFlags);

        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo allocCreateInfo;
        allocCreateInfo.pool = nullptr;
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocCreateInfo.priority = 1.f;
        allocCreateInfo.memoryTypeBits = 0;

        VkBuffer outBuffer;
       
        VkResult result = vmaCreateBuffer(m_VmaAllocator, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &outAllocation, nullptr);

        return outBuffer;
    }
} // namespace VkCore
