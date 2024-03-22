#include "SwapchainRenderPass.h"
#include "Devices/DeviceManager.h"
#include "Services/ServiceLocator.h"
#include "Texture/DepthImage.h"
#include "vulkan/vulkan.hpp"
#include "Swapchain.h"

namespace VkCore
{

    SwapchainRenderPass::SwapchainRenderPass(const Device& device)
    {
        vk::Extent2D swapExtent = device.GetSwapchain()->GetSwapExtent();
        m_DepthImage = DepthImage(swapExtent.width, swapExtent.height);

        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = device.GetSwapchain()->GetVkSurfaceFormat().surfaceFormat.format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentDescription depthAttachment;
        depthAttachment.format = m_DepthImage.GetFormat();
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
        vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::SubpassDescription subpass[] = {

            vk::SubpassDescription()
                .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                .setColorAttachmentCount(1)
                .setPColorAttachments(&colorReference)
                .setPDepthStencilAttachment(&depthReference)};

        vk::SubpassDependency dependency[] = {

            vk::SubpassDependency()
                .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                .setDstSubpass(0)
                .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                                 vk::PipelineStageFlagBits::eEarlyFragmentTests)
                .setSrcAccessMask(vk::AccessFlagBits::eNone)
                .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                                 vk::PipelineStageFlagBits::eEarlyFragmentTests)
                .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                                  vk::AccessFlagBits::eDepthStencilAttachmentWrite)};

        vk::RenderPassCreateInfo createInfo;

        vk::AttachmentDescription attachments[2] = {colorAttachment, depthAttachment};
        createInfo.setAttachments(attachments).setSubpasses(subpass).setDependencies(dependency);

        m_RenderPass = device.CreateRenderPass(createInfo);
    }
    void SwapchainRenderPass::Destroy()
    {
        m_DepthImage.Destroy();
        DeviceManager::GetDevice().DestroyRenderPass(m_RenderPass);
    }
} // namespace VkCore
