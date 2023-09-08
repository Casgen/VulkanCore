#pragma once

#include "vk_mem_alloc.h"

#include "../../Devices/Device.h"
#include "../../Devices/PhysicalDevice.h"
#include "IAllocatorService.h"
#include <cstdint>

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

        void CopyBuffer(const vk::Buffer& srcBuffer, const vk::Buffer& dstBuffer, const uint32_t size, const uint32_t srcOffset = 0, const uint32_t dstOffset = 0);
    };

} // namespace VkCore
