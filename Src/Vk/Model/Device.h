#pragma once

#include <memory>

#include "PhysicalDevice.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"

namespace VkCore
{

    class Swapchain;
    class Device
    {
      public:
        Device();

        Device(const PhysicalDevice& physicalDevice, const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME});

        ~Device();

        /**
         * @brief Initializes a Swapchain Object. THIS HAS TO BE CALLED AFTER CREATING A LOGICAL DEVICE!
         * @param physicalDevice
         */
        void InitSwapChain(const PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const uint32_t desiredWidth,
                           const uint32_t desiredHeight);

        vk::SwapchainKHR CreateSwapchain(const vk::SwapchainCreateInfoKHR createInfo);
        vk::ImageView CreateImageView(const vk::ImageViewCreateInfo& createInfo);

        // ---------- DESTROYERS -----------

        void DestroyImageView(const vk::ImageView& imageView);
        void DestroySwapchain(const vk::SwapchainKHR& swapchain);

        // ------------ GETTERS ------------

        vk::Device& GetDevice();
        std::vector<vk::Image> GetSwapchainImages(const vk::SwapchainKHR& swapchain);

      private:
        std::shared_ptr<Swapchain> m_Swapchain;
        vk::Device m_Device;
    };

} // namespace VkCore
