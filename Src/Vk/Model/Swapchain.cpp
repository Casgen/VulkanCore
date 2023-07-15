#include <cstdint>

#include "../../Log/Log.h"
#include "GLFW/glfw3.h"
#include "Swapchain.h"

namespace VkCore
{
    Swapchain::Swapchain(Device& device, const vk::SurfaceKHR& surface, const QueueFamilyIndices indices,
                         const SwapChainSupportDetails& supportDetails, const uint32_t desiredWidth,
                         const uint32_t desiredHeight)
    {

        // Choose a Swapchain format, present mode and extent
        vk::SurfaceFormat2KHR surfaceFormat = ChooseSurfaceFormat(supportDetails.m_SurfaceFormats);
        vk::PresentModeKHR presentMode = ChoosePresentMode(supportDetails.m_PresentModes);
        vk::Extent2D extent = ChooseSwapExtent(supportDetails.m_Capabilites, desiredWidth, desiredHeight);

        uint32_t imageCount = supportDetails.m_Capabilites.minImageCount + 1;

        // Checks whether the imageCount taken from the minImageCount doesn't exceed the maxImageCount.
        // if it does, it uses the maxImageCount instead.
        if (supportDetails.m_Capabilites.maxImageCount > 0 &&
            supportDetails.m_Capabilites.maxImageCount < imageCount)
        {
            imageCount = supportDetails.m_Capabilites.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo;

        createInfo.setSurface(surface);
        createInfo.setMinImageCount(imageCount);
        createInfo.setImageFormat(surfaceFormat.surfaceFormat.format);
        createInfo.setImageColorSpace(surfaceFormat.surfaceFormat.colorSpace);
        createInfo.setImageExtent(extent);
        createInfo.setImageArrayLayers(1);
        createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        uint32_t queueFamilyIndices[] = {indices.m_GraphicsFamily.value(), indices.m_PresentFamily.value()};

        if (indices.m_GraphicsFamily != indices.m_PresentFamily)
        {
            createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            createInfo.setQueueFamilyIndexCount(2);
            createInfo.setQueueFamilyIndices(queueFamilyIndices);
        }
        else
        {
            createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        }

        createInfo.setPreTransform(supportDetails.m_Capabilites.currentTransform);
        createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        createInfo.setPresentMode(presentMode);
        createInfo.setClipped(VK_TRUE);

        TRY_CATCH_BEGIN()

        m_Swapchain = device.CreateSwapchain(createInfo);
        m_Images = device.GetSwapchainImages(m_Swapchain);

        vk::ImageViewCreateInfo imageViewCreateInfo({}, {}, vk::ImageViewType::e2D, surfaceFormat.surfaceFormat.format,
                                                    vk::ComponentMapping(),
                                                    {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        for (const auto& image : m_Images)
        {
            imageViewCreateInfo.setImage(image);
            m_ImageViews.push_back(device.CreateImageView(imageViewCreateInfo));
        }

        TRY_CATCH_END()

        m_SwapExtent = extent;
        m_SurfaceFormat = surfaceFormat.surfaceFormat;
    }

    void Swapchain::Destroy(Device& device)
    {

        for (const auto& imageView : m_ImageViews)
        {
            device.DestroyImageView(imageView);
        }

        device.DestroySwapchain(m_Swapchain);
    }

    vk::SwapchainKHR Swapchain::GetVkSwapchain() const
    {
        return m_Swapchain;
    }

    vk::SurfaceFormat2KHR Swapchain::GetVkSurfaceFormat() const
    {
        return m_SurfaceFormat;
    }

    vk::Extent2D Swapchain::GetSwapExtent() const
    {
        return m_SwapExtent;
    }

    vk::SurfaceFormat2KHR Swapchain::ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats)
    {
        for (const auto& format : surfaceFormats)
        {
            if (format.format == vk::Format::eB8G8R8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return format;
            }
        }

        LOGF(Vulkan, Warning,
             "The appropriate surface format was not found! Using the first one in the array! (format = %s, colorSpace "
             "= %s)",
             vk::to_string(surfaceFormats[0].format).c_str(),
             vk::to_string(surfaceFormats[0].colorSpace).c_str())
        return surfaceFormats[0];
    }
    vk::PresentModeKHR Swapchain::ChoosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes)
    {
        for (const auto& mode : presentModes)
        {
            if (mode == vk::PresentModeKHR::eMailbox)
                return mode;
        }

        LOG(Vulkan, Warning, "Mailbox Presentmode was not found! Using the FIFO present mode.")
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D Swapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
                                             const uint32_t desiredWidth, const uint32_t desiredHeight)
    {
        vk::SurfaceCapabilitiesKHR surfaceCaps = capabilities;

        if (surfaceCaps.currentExtent.width !=
            std::max(surfaceCaps.currentExtent.width, surfaceCaps.maxImageExtent.width))
        {
            LOGF(Vulkan, Verbose, "Defined swap extent %dx%d", surfaceCaps.currentExtent.width,
                 surfaceCaps.currentExtent.height)
            return surfaceCaps.currentExtent;
        }

        vk::Extent2D actualExtent(desiredWidth, desiredHeight);

        actualExtent.width =
            std::clamp(actualExtent.width, surfaceCaps.minImageExtent.width, surfaceCaps.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, surfaceCaps.minImageExtent.height, surfaceCaps.maxImageExtent.height);

        LOGF(Vulkan, Verbose, "Defined swap extent %dx%d", actualExtent.width, actualExtent.height)

        return actualExtent;
    }
} // namespace VkCore
