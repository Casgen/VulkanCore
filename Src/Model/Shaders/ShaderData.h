#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>

namespace VkCore
{
    struct ShaderData
    {

        vk::ShaderStageFlagBits m_StageFlags;
        std::vector<uint32_t> m_Data;
    };
} // namespace VkCore
