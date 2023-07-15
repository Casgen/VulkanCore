#include "Device.h"
#include "../../Log/Log.h" // IMPORTANT! Always include this when using the TRY_CATCH_BEGIN and TRY_CATCH_END macro
#include "../Utils.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace VkCore
{

    Device::Device()
    {
    }

    Device::Device(const PhysicalDevice& physicalDevice, const std::vector<const char*> deviceExtensions)
    {
        QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();

        // TODO: Maybe take care of the priorities somehow in the future?
        const float queuePriority = 1.f;

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {physicalDevice.GetQueueFamilyIndices().m_GraphicsFamily.value(),
                                                  physicalDevice.GetQueueFamilyIndices().m_GraphicsFamily.value()};

        for (uint32_t queueFamily : uniqueQueueFamilies)
        {

            queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(),
                                                                 indices.m_GraphicsFamily.value(), 1, &queuePriority));
        }

        TRY_CATCH_BEGIN()

        std::vector<const char*> layerExtensions;

#ifdef DEBUG
        layerExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        m_Device = physicalDevice.CreateDevice(
            vk::DeviceCreateInfo(vk::DeviceCreateFlags(), queueCreateInfos, layerExtensions, deviceExtensions));

        TRY_CATCH_END()

        LOG(Vulkan, Info, "Logical device created.")
    }

    void Device::InitSwapChain(const PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface,
                               const uint32_t desiredWidth, const uint32_t desiredHeight)
    {
        m_Swapchain =
            std::make_shared<Swapchain>(*this, surface, physicalDevice.GetQueueFamilyIndices(),
                                        physicalDevice.GetSwapChainSupportDetails(), desiredWidth, desiredHeight);
    }

    Device::~Device()
    {
        m_Swapchain->Destroy(*this);
        m_Device.destroy();
    }

    vk::RenderPass Device::CreateSwapchainRenderPass()
    {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = m_Swapchain->GetVkSurfaceFormat().surfaceFormat.format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        // Before drawing a new frame, this ensures that the frambuffer is cleared to black
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        // After rendering, the contents will be stored in memory with STORE_OP_STORE
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;

        // This is used for the depth (stencil) buffer (not used here right now because we aren't rendering in 3D)
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        vk::SubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = vk::AccessFlagBits::eNone;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        TRY_CATCH_BEGIN()

        return m_Device.createRenderPass(renderPassInfo);
        
        TRY_CATCH_END()
    }

    vk::SwapchainKHR Device::CreateSwapchain(const vk::SwapchainCreateInfoKHR createInfo)
    {
        return m_Device.createSwapchainKHR(createInfo);
    }
    vk::ImageView Device::CreateImageView(const vk::ImageViewCreateInfo& createInfo)
    {
        return m_Device.createImageView(createInfo);
    }

    void Device::DestroyImageView(const vk::ImageView& imageView)
    {
        m_Device.destroyImageView(imageView);
    }

    void Device::DestroySwapchain(const vk::SwapchainKHR& swapchain)
    {
        m_Device.destroySwapchainKHR(swapchain);
    }

    vk::Device& Device::GetVkDevice()
    {
        return m_Device;
    }

    std::vector<vk::Image> Device::GetSwapchainImages(const vk::SwapchainKHR& swapchain)
    {
        return m_Device.getSwapchainImagesKHR(swapchain);
    }
} // namespace VkCore
