#pragma once

#include "vulkan/vulkan.hpp"

namespace VkCore
{
    class RenderPass
    {
      public:
        RenderPass()
        {
        }
        RenderPass(const vk::RenderPass renderPass) : m_RenderPass(renderPass)
        {
        }

        // ------------- GETTERS -----------------

        vk::RenderPass GetRenderPass() const
        {
            return m_RenderPass;
        }

      protected:
        vk::RenderPass m_RenderPass;
    };

} // namespace VkCore
