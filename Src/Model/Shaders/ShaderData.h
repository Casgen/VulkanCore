#pragma once
#include <cstdint>

#include "vulkan/vulkan.hpp"

namespace VkCore
{
    struct ShaderData
    {

        vk::ShaderStageFlagBits m_StageFlags;
        std::vector<uint32_t> m_Data;
    };
} // namespace VkCore
