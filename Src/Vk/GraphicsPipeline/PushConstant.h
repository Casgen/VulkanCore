#pragma once

#include "vulkan/vulkan.hpp"

namespace VkCore
{
    class PushConstants
    {
        PushConstants() : m_PushConstantRanges({}) {}

        std::vector<vk::PushConstantRange> m_PushConstantRanges;
    };
} // namespace VkCore
