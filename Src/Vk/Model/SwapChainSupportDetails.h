#pragma once

#include <vector>
#include "vulkan/vulkan_structs.hpp"

namespace VkCore
{

    struct SwapChainSupportDetails
    {

        vk::SurfaceCapabilities2KHR m_Capabilites;
        vk::PhysicalDeviceSurfaceInfo2KHR m_SurfaceInfo;
        std::vector<vk::SurfaceFormat2KHR> m_SurfaceFormats;
        std::vector<vk::PresentModeKHR> m_PresentModes;
    };

} // Namespace VkCore
