#pragma once

#include "Devices/Device.h"
#include "Texture/DepthImage.h"
#include "vulkan/vulkan_handles.hpp"

namespace VkCore
{

    class SwapchainRenderPass
    {
      public:
        SwapchainRenderPass(){};
        SwapchainRenderPass(const Device& device);
        void Destroy();

        DepthImage& GetDepthImage()
        {
            return m_DepthImage;
        }

        vk::RenderPass GetVkRenderPass() const
        {
            return m_RenderPass;
        }

      private:
        vk::RenderPass m_RenderPass;
        DepthImage m_DepthImage;
    };

} // namespace VkCore
