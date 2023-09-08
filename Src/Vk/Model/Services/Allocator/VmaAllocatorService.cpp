#include <cstring>
#include <iterator>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_structs.hpp>

#include "VmaAllocatorService.h"
#include "../../../Utils.h"
#include "../../../../Log/Log.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"

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

        VkBufferCreateInfo srcBufferInfo;
        srcBufferInfo.size = dataSize;
        srcBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        VmaAllocationCreateInfo stagingAllocInfo;
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags =
            VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer srcBuffer;
        VmaAllocation srcAllocation;

        Utils::CheckVkResult(
            vmaCreateBuffer(m_VmaAllocator, &srcBufferInfo, &stagingAllocInfo, &srcBuffer, &srcAllocation, nullptr));

        // Copy the data from the memory to the staging buffer.
        void* mapData;

        vmaMapMemory(m_VmaAllocator, srcAllocation, &mapData);
        std::memcpy(mapData, buffer.GetData(), dataSize);
        vmaUnmapMemory(m_VmaAllocator, srcAllocation);

        // Create an only GPU-accessible buffer.
        VkBufferCreateInfo dstBufferInfo;
        dstBufferInfo.size = dataSize;
        dstBufferInfo.usage = static_cast<uint32_t>(buffer.GetUsageFlags()) | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VmaAllocationCreateInfo gpuAllocInfo;
        gpuAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        gpuAllocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

        VkBuffer dstBuffer;
        VmaAllocation dstAllocation;

        Utils::CheckVkResult(
            vmaCreateBuffer(m_VmaAllocator, &dstBufferInfo, &gpuAllocInfo, &dstBuffer, &dstAllocation, nullptr));

        CopyBuffer(srcBuffer, dstBuffer, dataSize);

        vmaDestroyBuffer(m_VmaAllocator, srcBuffer, srcAllocation);

        buffer.SetVkBuffer(dstBuffer);
        buffer.SetVmaAllocation(dstAllocation);

        LOG(Allocation, Verbose, "Buffer has been allocated.")
    }

    void VmaAllocatorService::CopyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const uint32_t size, const uint32_t srcOffset, const uint32_t dstOffset)
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
} // namespace VkCore
