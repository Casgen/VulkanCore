#pragma once

#include "IAllocatorService.h"

namespace VkCore
{

    class NullAllocatorService : public IAllocatorService
    {
      public:
        VkBuffer CreateBuffer(const Buffer::BufferInfo& bufferInfo, VmaAllocation& outAllocation,
                              VmaAllocationInfo* outAllocationInfo = nullptr) override;
        void DestroyBuffer(Buffer& buffer) override;

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

        VkBuffer CreateBufferOnGpu(const void* data, const Buffer::BufferInfo bufferInfo, VmaAllocation& allocation,
                                   VmaAllocationInfo* allocationInfo) override;

        /**
         * @brief Maps the buffer memory and returns back a pointer to the VkBuffer memory. It can be used for updating the data
         */
        void MapMemory(const VmaAllocation& allocation, void* mappedPtr) override;

        /**
         * @brief unmaps the buffer memory, making the mapped pointer invalid.
         */
        void UnmapMemory(const VmaAllocation& allocation) override;
    };

} // namespace VkCore
