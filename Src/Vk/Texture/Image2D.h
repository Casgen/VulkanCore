#pragma once

#include "../Buffers/Buffer.h"
#include "ImageData.h"
#include "../../Vk/Devices/Device.h"
#include "vulkan/vulkan_enums.hpp"
#include <cstdint>

namespace VkCore
{
    class Image2D
    {
      public:
        Image2D(){};
        Image2D(const char* filename);

        void InitializeOnTheGpu(const uint32_t mipLevels = 1);
        void TransitionImageLayout(const VkCore::Device& device, const vk::Format format,
                                   const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout);

      private:
        Buffer m_Buffer;
        ImageData m_ImgData;

        vk::Image m_Image;
        VmaAllocation m_Allocation;
        VmaAllocationInfo m_AllocationInfo;
    };

} // namespace VkCore
