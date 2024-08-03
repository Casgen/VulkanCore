#pragma once

#include "IAllocatorService.h"

namespace VkCore
{

    class NullAllocatorService : public IAllocatorService
    {
      public:
        VkBuffer CreateBuffer(const size_t size, const std::vector<uint32_t> queueFamilyIndices,
                              const vk::BufferUsageFlags usageFlags, const vk::BufferCreateFlags createFlags,
                              const VmaMemoryUsage memoryUsage, const VmaAllocationCreateFlags allocFlags,
                              VmaAllocation& outAllocation, VmaAllocationInfo* outAllocationInfo) override;
        /**
         * @brief Allocates and creates a new VkImage.
         * @param data - pointer to the data
         * @param size - size of the data
         * @param createInfo
         * @param outAllocation
         * #param outAlloationInfo - optional
         */
        VkImage CreateImage(const void* data, const VkDeviceSize size, const vk::ImageCreateInfo& createInfo,
                            const VmaAllocationCreateInfo& allocCreateInfo, VmaAllocation& outAllocation,
                            VmaAllocationInfo* outAllocationInfo = nullptr) override;

        VkImage CreateImage(const VkDeviceSize size, const vk::ImageCreateInfo& createInfo,
                            const VmaAllocationCreateInfo& allocCreateInfo, VmaAllocation& outAllocation,
                            VmaAllocationInfo* outAllocationInfo) override;
        /**
         * @brief Allocates creates a new VkImage.
         * @param width
         * @param height
         * @param format - Specifies the image format
         * @param tiling
         * @param usageFlags - How is the image going to be used
         * @param outAllocation - Output Allocation struct
         * #param outAlloationInfo - output allocation info struct (optional)
         */
        VkImage CreateImage(const uint32_t width, const uint32_t height, const vk::Format format,
                            vk::ImageTiling imageTiling, vk::ImageUsageFlags usageFlags,
                            const VmaAllocationCreateInfo& allocCreateInfo, VmaAllocation& outAllocation,
                            VmaAllocationInfo* outAllocationInfo = nullptr) override;

        void DestroyBuffer(Buffer& buffer) override;
        void DestroyImage(vk::Image& image, VmaAllocation& allocation) override;

        /**
         * @brief Copies the data from the source buffer to the destination buffer. A Command buffer is used to transfer
         * the data with a transfer queue.
         * @param srcBuffer - source buffer to copy
         * @param dstBuffer - destination buffer to paste the data into
         * @param size - how many bytes should be copied
         * @param srcOffset - the offset in bytes to start the copy from the source buffer
         * @param dstOffset - the offset in bytes to paste the data into the destination buffer
         */
        void CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const size_t size,
                        const uint32_t srcOffset = 0, const uint32_t dstOffset = 0) override;

        void CopyBufferToImage(const VkImage& image, const VkBuffer& srcBuffer, const VkDeviceSize size,
                               const vk::Extent2D& resolution) override;

        VkBuffer CreateBufferOnGpu(const void* data, const size_t size, const vk::BufferUsageFlags usageFlags,
                                   VmaAllocation& allocation, VmaAllocationInfo* allocationInfo) override;

        void UpdateBufferOnGpu(const Buffer& buffer, const void* data, size_t size) override;

        /**
         * @brief Maps the buffer memory and returns back a pointer to the VkBuffer memory. It can be used for updating
         * the data
         */
        void MapMemory(const VmaAllocation& allocation, void* mappedPtr) override;

        /**
         * @brief unmaps the buffer memory, making the mapped pointer invalid.
         */
        void UnmapMemory(const VmaAllocation& allocation) override;
    };

} // namespace VkCore
