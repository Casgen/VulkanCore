#pragma once

#include "../../Buffers/Buffer.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_structs.hpp"

namespace VkCore
{
    class IAllocatorService
    {
      public:
        friend class Buffer;

        virtual ~IAllocatorService() {};

        /**
         * @brief takes data and info from the buffer and creates a new GPU-only visible buffer (allocates and populates
         * it).
         * @param - A reference to an existing buffer. Used for obtaining info about the buffer.
         * @param - pointer to the data.
         */
        virtual VkBuffer CreateBufferOnGpu(const void* data, const size_t size, const vk::BufferUsageFlags usageFlags,
                                           VmaAllocation& allocation, VmaAllocationInfo* allocationInfo) = 0;

        /**
         * @brief Allocates and creates a new buffer. Note that no data is being transferred!
         * @param bufferInfo - Struct containg information for creating a buffer
         * @param outAllocation - Output VmaAllocation struct.
         * @param outAllocationInfo - Optional output VmaAllocationInfo struct.
         * @return newly created VkBuffer handle.
         */
        virtual VkBuffer CreateBuffer(const size_t size, const std::vector<uint32_t> queueFamilyIndices,
                                      const vk::BufferUsageFlags usageFlags, const vk::BufferCreateFlags createFlags,
                                      const VmaMemoryUsage memoryUsage, const VmaAllocationCreateFlags allocFlags,
                                      VmaAllocation& outAllocation, VmaAllocationInfo* outAllocationInfo) = 0;
        /**
         * @brief Allocates and creates a new VkImage. Data is also transferred.
         * @param data - pointer to the data
         * @param size - size of the data
         * @param createInfo
         * @param outAllocation
         * #param outAlloationInfo - optional
         */
        virtual VkImage CreateImage(const void* data, const VkDeviceSize size, const vk::ImageCreateInfo& createInfo,
                                    const VmaAllocationCreateInfo& allocCreateInfo, VmaAllocation& outAllocation,
                                    VmaAllocationInfo* outAllocationInfo = nullptr) = 0;
        /**
         * @brief Allocates and creates a new VkImage.
         * @param data - pointer to the data
         * @param size - size of the data
         * @param createInfo
         * @param outAllocation
         * #param outAlloationInfo - optional
         */
        virtual VkImage CreateImage(const VkDeviceSize size, const vk::ImageCreateInfo& createInfo,
                                    const VmaAllocationCreateInfo& allocCreateInfo, VmaAllocation& outAllocation,
                                    VmaAllocationInfo* outAllocationInfo = nullptr) = 0;
        /**
         * @brief Allocates creates a new VkImage.
         * @param width
         * @param height
         * @param createInfo
         * @param outAllocation
         * #param outAlloationInfo - optional
         */
        virtual VkImage CreateImage(const uint32_t width, const uint32_t height, const vk::Format format,
                                    vk::ImageTiling imageTiling, vk::ImageUsageFlags usageFlags,
                                    const VmaAllocationCreateInfo& allocCreateInfo, VmaAllocation& outAllocation,
                                    VmaAllocationInfo* outAllocationInfo = nullptr) = 0;

        virtual void CopyBufferToImage(const VkImage& image, const VkBuffer& srcBuffer, const VkDeviceSize size,
                                       const vk::Extent2D& resolution) = 0;
        /**
         * Destroys the buffer and frees the memory
         * @param buffer - buffer to destroy
         */
        virtual void DestroyBuffer(Buffer& buffer) = 0;

        /**
         * Destroys the image and frees the memory.
         * @param image - image to destroy.
         * @param allocation - image allocation associated with it.
         */
        virtual void DestroyImage(vk::Image& image, VmaAllocation& allocation) = 0;

        /**
         * @brief Copies the data from the source buffer to the destination buffer. A Command buffer is used to transfer
         * the data with a transfer queue.
         * @param srcBuffer - source buffer to copy
         * @param dstBuffer - destination buffer to paste the data into
         * @param size - how many bytes should be copied
         * @param srcOffset - the offset in bytes to start the copy from the source buffer
         * @param dstOffset - the offset in bytes to paste the data into the destination buffer
         */
        virtual void CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const size_t size,
                                const uint32_t srcOffset = 0, const uint32_t dstOffset = 0) = 0;
        /**
         * @brief Maps the buffer memory and returns back a pointer to the VkBuffer memory. It can be used for updating
         * the data
         */
        virtual void MapMemory(const VmaAllocation& allocation, void* mappedPtr) = 0;

        /**
         * @brief unmaps the buffer memory, making the mapped pointer invalid.
         */
        virtual void UnmapMemory(const VmaAllocation& allocation) = 0;
    };

} // namespace VkCore
