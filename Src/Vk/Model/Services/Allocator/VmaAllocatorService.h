#pragma once
#include <cstdint>

#include "../../Devices/Device.h"
#include "../../Devices/PhysicalDevice.h"
#include "IAllocatorService.h"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan_core.h"

namespace VkCore
{
    class VmaAllocatorService : public IAllocatorService
    {
      public:
        VmaAllocatorService(Device& device, PhysicalDevice& physicalDevice, vk::Instance& instance);
        ~VmaAllocatorService();

        void DestroyBuffer(Buffer& buffer) override;

        /**
         * @brief Allocates and creates a new buffer.
         * @param data - pointer to the data.
         * @param bufferInfo - Struct containg information for creating a buffer
         * @param outAllocation - Output VmaAllocation struct.
         * @param outAllocationInfo - Optional output VmaAllocationInfo struct.
         * @return newly created buffer.
         */
        VkBuffer CreateBuffer(const Buffer::BufferInfo& bufferInfo, VmaAllocation& outAllocation,
                              VmaAllocationInfo* outAllocationInfo = nullptr) override;
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

        /**
         *  @brief Allocated a buffer onto the GPU device, making it only writable/readable for the GPU. It cannot be
         *  accessed by the CPU!
         *  @param buffer - A block of memory to copy the data from and transfer to the GPU.
         *  @param data - pointer to the data.
         */
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

      private:
        VmaAllocator m_VmaAllocator;

        PhysicalDevice m_PhysicalDevice;
        Device m_Device;
    };

} // namespace VkCore
