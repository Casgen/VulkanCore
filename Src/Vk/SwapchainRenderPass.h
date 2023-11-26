#pragma once

#include "RenderPass.h"
#include "Devices/Device.h"

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
