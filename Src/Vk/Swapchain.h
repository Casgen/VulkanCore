#pragma once

#include "Devices/Device.h"
#include "SwapChainSupportDetails.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace VkCore
{

    class Swapchain
    {

      public:
        Swapchain(Device& device, const vk::SurfaceKHR& surface, const QueueFamilyIndices indices,
                  const SwapChainSupportDetails& supportDetails, const uint32_t desiredWidth,
                  const uint32_t desiredHeight);
        Swapchain() = default;

        /**
         * @brief Destroy the swapchain. It has to be destroyed with the Device object in order to destroy image views
         * and the Vulkan swapchain.
         * @param device
         */
        void Destroy(Device& device);

        // ------------ GETTERS ------------

        vk::SwapchainKHR GetVkSwapchain() const;
        uint32_t GetImageCount() const;
        vk::SurfaceFormat2KHR GetVkSurfaceFormat() const;
        vk::Extent2D GetSwapExtent() const;
        std::vector<vk::ImageView> GetImageViews() const;
        uint32_t GetNumberOfSwapBuffers() const;

      private:
        vk::SwapchainKHR m_Swapchain;

        std::vector<vk::Image> m_Images;
        std::vector<vk::ImageView> m_ImageViews;

        vk::SurfaceFormat2KHR m_SurfaceFormat;
        vk::Extent2D m_SwapExtent;

        vk::SurfaceFormat2KHR ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats);
        vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& presentModes, const uint32_t desiredWidth,
                                      const uint32_t desiredHeight, const vk::SurfaceKHR& surface);
    };
} // namespace VkCore
