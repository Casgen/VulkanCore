#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <cstdint>
#include <vector>

namespace VkCore
{

    class Swapchain
    {

      public:
        Swapchain(const vk::SurfaceKHR& surface, const uint32_t desiredWidth, const uint32_t desiredHeight);
        Swapchain() = default;

        /**
         * @brief Destroy the swapchain. It has to be destroyed with the Device object in order to destroy image views
         * and the Vulkan swapchain.
         * @param device
         */
        void Destroy();

        vk::SwapchainKHR operator*() const
        {
            return m_Swapchain;
        }

        // ------------ GETTERS ------------

        vk::SwapchainKHR GetVkSwapchain() const;
        uint32_t GetImageCount() const;
        vk::SurfaceFormat2KHR GetVkSurfaceFormat() const;
        vk::PresentModeKHR GetPresentMode() const;
        vk::Extent2D GetSwapExtent() const;
        std::vector<vk::ImageView> GetImageViews() const;
        std::vector<vk::Image> GetImages() const;
        uint32_t GetNumberOfSwapBuffers() const;

        /**
         *  Acquires the next image index according to the framebuffer in the swapchain.
         *  @param semaphore - semaphore which will be signaled.
         *  @param fence - fence which will be signaled.
         *  @param timeout - how long should it wait for the acquisition of the image before bailing.
         */
        vk::ResultValue<uint32_t> AcquireNextImageKHR(const vk::Semaphore& semaphore, const vk::Fence& fence = {},
                                                      const uint64_t timeout = UINT64_MAX);

      private:
        vk::SwapchainKHR m_Swapchain = nullptr;

        std::vector<vk::Image> m_Images = {};
        std::vector<vk::ImageView> m_ImageViews = {};

        vk::SurfaceFormat2KHR m_SurfaceFormat;
        vk::Extent2D m_SwapExtent;
        vk::PresentModeKHR m_PresentMode;

        vk::SurfaceFormat2KHR ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats);
        vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceKHR& surface, const vk::SurfaceCapabilitiesKHR& presentModes,
                                      const uint32_t desiredWidth, const uint32_t desiredHeight);
    };
} // namespace VkCore
