#pragma once

#include "RenderPass.h"
#include "Swapchain.h"

namespace VkCore
{

    class SwapchainRenderPass : public RenderPass
    {
      public:
        SwapchainRenderPass()
        {
        }
        SwapchainRenderPass(const Device& device);
    };

} // namespace VkCore
