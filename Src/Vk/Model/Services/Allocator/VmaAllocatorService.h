#pragma once
#include <cstdint>

#include "../../Devices/Device.h"
#include "../../Devices/PhysicalDevice.h"
#include "IAllocatorService.h"
#include "vk_mem_alloc.h"

namespace VkCore
{
    class VmaAllocatorService : public IAllocatorService
    {
      public:
        VmaAllocatorService(Device& device, PhysicalDevice& physicalDevice, vk::Instance& instance);
        ~VmaAllocatorService();

        /**
         *  @brief Allocated a buffer onto the GPU device, making it only writable/readable for the GPU. It cannot be
         *  accessed by the CPU!
         *  @param buffer - A block of memory to copy the data from and transfer to the GPU.
         */
        void AllocateBufferOnGPU(Buffer& buffer) override;

      private:
        VmaAllocator m_VmaAllocator;

        PhysicalDevice m_PhysicalDevice;
        Device m_Device;

        void CopyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const uint32_t size,
                        const uint32_t srcOffset = 0, const uint32_t dstOffset = 0);

        /**
         * @brief Allocates and creates a new buffer. After creation, no data is being transfered!
         * @param size - size of the data in bytes.
         * @param usageFlags - how or what is the buffer used for.
         * @param allocFlags - Create flags to use when allocating a buffer. (see VmaAllocationCreateFlags)
         * @param outAllocation - Output VmaAllocation struct.
         * @param sharingMode - indicates whether the buffer will be used in one queue or shared between more queues.
         * @param bufferCreateFlags - Buffer creation flags
         * @param memoryUsage - How the memory should be used. (VMA_MEMORY_USAGE_AUTO is the default)
         * @return newly created buffer.
         */
        VkBuffer CreateBuffer(const size_t size, vk::BufferUsageFlags usageFlags,
                              VmaAllocationCreateFlags allocFlags, VmaAllocation& outAllocation,
                              vk::SharingMode sharingMode = vk::SharingMode::eExclusive,
                              vk::BufferCreateFlags bufferCreateFlags = {}, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO);
    };

} // namespace VkCore
