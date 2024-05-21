#include <cstddef>
#include <cstdint>
#include <cstring>

#include <stdexcept>
#include "../../Devices/DeviceManager.h"
#include "vulkan/vulkan.hpp"

#include "VmaAllocatorService.h"
#include "../../Utils.h"
#include "../../../Log/Log.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

// Include it always all the way down!
#include <vk_mem_alloc.h>

namespace VkCore
{
    VmaAllocatorService::VmaAllocatorService(vk::Instance& instance)

    {
        VmaAllocatorCreateInfo createInfo;

        // TODO: Refine the API Version definition
        createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        createInfo.device = *DeviceManager::GetDevice();
        createInfo.physicalDevice = *DeviceManager::GetPhysicalDevice();
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
        if (buffer.GetVkBuffer() != VK_NULL_HANDLE && buffer.GetVmaAllocation() != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_VmaAllocator, buffer.GetVkBuffer(), buffer.GetVmaAllocation());
            return;
        }

        const char* errorMsg = "Failed to destroy a buffer! Either the VkBuffer or VmaAllocation is NULL!";
        LOG(Vulkan, Error, errorMsg)
    }

    void VmaAllocatorService::DestroyImage(vk::Image& image, VmaAllocation& allocation)
    {
        vmaDestroyImage(m_VmaAllocator, image, allocation);
    }

    void VmaAllocatorService::CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const size_t size,
                                         const uint32_t srcOffset, const uint32_t dstOffset)
    {
        // Create a Transient pool only for this transfer command.
        vk::CommandPool cmdPool;
        vk::CommandBuffer cmdBuffer = DeviceManager::GetDevice().BeginSingleTimeCommands(cmdPool);

        vk::BufferCopy copyRegion{srcOffset, dstOffset, size};

        cmdBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

        DeviceManager::GetDevice().EndSingleTimeCommands(cmdBuffer, cmdPool);
    }

    void VmaAllocatorService::CopyBufferToImage(const VkImage& image, const VkBuffer& srcBuffer,
                                                const VkDeviceSize size, const vk::Extent2D& resolution)
    {
        // Create a Transient pool only for this transfer command.
        vk::CommandPool cmdPool;
        vk::CommandBuffer cmdBuffer = DeviceManager::GetDevice().BeginSingleTimeCommands(cmdPool);

        vk::BufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        copyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;

        copyRegion.imageOffset = vk::Offset3D{0, 0, 0};
        copyRegion.imageExtent = vk::Extent3D{resolution.width, resolution.height, 0};

        cmdBuffer.copyBufferToImage(srcBuffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);

        DeviceManager::GetDevice().EndSingleTimeCommands(cmdBuffer, cmdPool);
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

    VkImage VmaAllocatorService::CreateImage(const void* data, const VkDeviceSize size,
                                             const vk::ImageCreateInfo& createInfo,
                                             const VmaAllocationCreateInfo& allocCreateInfo,
                                             VmaAllocation& outAllocation, VmaAllocationInfo* outAllocationInfo)
    {
        if (size <= 0)
        {
            LOGF(Vulkan, Fatal,
                 "Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)! Given size was %d", size)
            throw std::runtime_error("Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)!");
        }

        if (data == nullptr)
        {
            const char* errorMsg = "Couldn't allocate buffer on the GPU! Pointer to the data is nullptr!";
            LOG(Vulkan, Fatal, errorMsg)
            throw std::runtime_error(errorMsg);
        }

        VkImage image = VK_NULL_HANDLE;

        TRY_CATCH_BEGIN()

        vmaCreateImage(m_VmaAllocator, reinterpret_cast<const VkImageCreateInfo*>(&createInfo), &allocCreateInfo,
                       &image, &outAllocation, outAllocationInfo);

        TRY_CATCH_END()

        return image;
    }

    VkImage VmaAllocatorService::CreateImage(const uint32_t width, const uint32_t height, const vk::Format format,
                                             vk::ImageTiling imageTiling, vk::ImageUsageFlags usageFlags,
                                             const VmaAllocationCreateInfo& allocCreateInfo,
                                             VmaAllocation& outAllocation, VmaAllocationInfo* outAllocationInfo)
    {
        if (width <= 0 || height <= 0)
        {
            LOGF(Vulkan, Fatal,
                 "Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)! Given resolution was %dx%d",
                 width, height)
            throw std::runtime_error("Couldn't allocate buffer on the GPU! Buffer size is invalid! (size <= 0)!");
        }

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.tiling = static_cast<VkImageTiling>(imageTiling);
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = static_cast<VkImageUsageFlags>(usageFlags) | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.format = static_cast<VkFormat>(format);

        VkImage image = VK_NULL_HANDLE;

        TRY_CATCH_BEGIN()

        vmaCreateImage(m_VmaAllocator, &imageInfo, &allocCreateInfo, &image, &outAllocation, outAllocationInfo);

        TRY_CATCH_END()

        return image;
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

    void VmaAllocatorService::MapMemory(const VmaAllocation& allocation, void* mappedPtr)
    {
        vmaMapMemory(m_VmaAllocator, allocation, &mappedPtr);
    }

    void VmaAllocatorService::UnmapMemory(const VmaAllocation& allocation)
    {
        vmaUnmapMemory(m_VmaAllocator, allocation);
    }

} // namespace VkCore
