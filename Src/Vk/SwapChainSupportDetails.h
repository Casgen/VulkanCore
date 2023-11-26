#pragma once

#include <vector>
#include "vulkan/vulkan.hpp"

namespace VkCore
{

    struct SwapChainSupportDetails
    {

        vk::SurfaceCapabilitiesKHR m_Capabilites;
        vk::PhysicalDeviceSurfaceInfo2KHR m_SurfaceInfo;
        std::vector<vk::SurfaceFormatKHR> m_SurfaceFormats;
        std::vector<vk::PresentModeKHR> m_PresentModes;
    };

} // Namespace VkCore
