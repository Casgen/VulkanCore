#pragma once

#include "stb_image.h"
#include "vulkan/vulkan_core.h"

namespace VkCore
{
    class ImageData
    {
      public:
        uint32_t width, height, channels = 0;

        // ImageData manages the lifetime of the image binary data
        stbi_uc* data = nullptr;
        VkDeviceSize dataSize = 0;

        ImageData() = default;

        ImageData(const ImageData& other) = delete;
        ImageData(ImageData&& other);

        ImageData& operator=(const ImageData& other) = delete;
        ImageData& operator=(ImageData&& other);

        ~ImageData();

        void Free();
    };

} // namespace VkCore
