#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <sys/types.h>
#include <vector>
#include <set>

#include "Device.h"
#include "../../Log/Log.h" // IMPORTANT! Always include this when using the TRY_CATCH_BEGIN and TRY_CATCH_END macro
#include "PhysicalDevice.h"
#include "../Swapchain.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

static std::vector<const char*> layerExtensions{
#ifdef DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
};

namespace VkCore
{

    Device::Device()
    {
    }

    Device::Device(const PhysicalDevice& physicalDevice, std::vector<const char*> deviceExtensions)
    {

        QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();

        // TODO: Maybe take care of the priorities somehow in the future?
        const float queuePriority = 1.f;

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.m_GraphicsFamily.value(), indices.m_PresentFamily.value()};

        if (indices.m_ComputeFamily.has_value())
        {
            uniqueQueueFamilies.emplace(indices.m_ComputeFamily.value());
        }

        if (indices.m_TransferFamily.has_value())
        {
            uniqueQueueFamilies.emplace(indices.m_TransferFamily.value());
        }

        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            vk::DeviceQueueCreateInfo createInfo;

            createInfo.setQueueCount(1).setQueueFamilyIndex(queueFamily).setQueuePriorities(queuePriority);

            queueCreateInfos.emplace_back(createInfo);
        }

        TRY_CATCH_BEGIN()

        vk::PhysicalDeviceFeatures2 features2 = physicalDevice.GetPhysicalDeviceFeatures2();

        vk::PhysicalDeviceMeshShaderFeaturesNV meshShaderFeatures(true, true);

        vk::PhysicalDeviceVulkan12Features vulkan12Features{};
        vulkan12Features.setBufferDeviceAddress(true);
        vulkan12Features.setPNext(&meshShaderFeatures);

        vk::PhysicalDeviceFeatures2 deviceFeatures2{};
        deviceFeatures2.setPNext(&vulkan12Features);

        // Enable Device Buffer Address

        auto it = std::find_if(deviceExtensions.begin(), deviceExtensions.end(), [&](const char* const& item) {
            return strcmp(item, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) == 0;
        });

        if (it != deviceExtensions.end())
            deviceExtensions.erase(it);

        vk::DeviceCreateInfo deviceCreateInfo;

        deviceCreateInfo.setPEnabledExtensionNames(deviceExtensions)
            .setQueueCreateInfos(queueCreateInfos)
            .setPEnabledLayerNames(layerExtensions)
            .setPNext(&meshShaderFeatures);


        for (auto& ext : deviceExtensions)
        {

            if (strcmp(ext, VK_NV_MESH_SHADER_EXTENSION_NAME) == 0 ||
                strcmp(ext, VK_EXT_MESH_SHADER_EXTENSION_NAME) == 0)
            {
                deviceCreateInfo.setPNext(&meshShaderFeatures);
                break;
            }
        }

        m_Device = physicalDevice.CreateDevice(deviceCreateInfo);

        LOG(Vulkan, Info, "Logical device created.")

        InitQueues(indices);

        TRY_CATCH_END()
    }

    void Device::InitSwapChain(const PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface,
                               const uint32_t desiredWidth, const uint32_t desiredHeight)
    {
        m_Swapchain =
            std::make_shared<Swapchain>(*this, surface, physicalDevice.GetQueueFamilyIndices(),
                                        physicalDevice.GetSwapChainSupportDetails(), desiredWidth, desiredHeight);
    }

    void Device::Destroy()
    {
        m_Swapchain->Destroy(*this);
        m_Device.destroy();
    }

    vk::RenderPass Device::CreateRenderPass(const vk::RenderPassCreateInfo createInfo) const
    {

        TRY_CATCH_BEGIN()

        return m_Device.createRenderPass(createInfo);

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

    vk::DescriptorPool Device::CreateDescriptorPool(const vk::DescriptorPoolCreateInfo& createInfo)
    {
        return m_Device.createDescriptorPool(createInfo);
    }

    vk::DescriptorSetLayout Device::CreateDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo)
    {
        return m_Device.createDescriptorSetLayout(createInfo);
    }

    vk::CommandPool Device::CreateCommandPool(const vk::CommandPoolCreateInfo& createInfo)
    {
        return m_Device.createCommandPool(createInfo);
    }

    vk::ShaderModule Device::CreateShaderModule(const vk::ShaderModuleCreateInfo& createInfo)
    {
        return m_Device.createShaderModule(createInfo);
    }

    vk::PipelineLayout Device::CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo)
    {
        return m_Device.createPipelineLayout(createInfo);
    }

    vk::Framebuffer Device::CreateFrameBuffer(const vk::FramebufferCreateInfo& createInfo)
    {
        return m_Device.createFramebuffer(createInfo);
    }

    vk::ResultValue<std::vector<vk::Pipeline>> Device::CreateGraphicsPipelines(
        const std::vector<vk::GraphicsPipelineCreateInfo>& createInfo)
    {
        return m_Device.createGraphicsPipelines(nullptr, createInfo);
    }

    vk::Semaphore Device::CreateSemaphore(const vk::SemaphoreCreateInfo& createInfo)
    {
        return m_Device.createSemaphore(createInfo);
    }

    vk::Fence Device::CreateFence(const vk::FenceCreateInfo& createInfo)
    {
        return m_Device.createFence(createInfo);
    }

    void Device::DestroyImageView(const vk::ImageView& imageView)
    {
        m_Device.destroyImageView(imageView);
    }

    void Device::DestroySwapchain(const vk::SwapchainKHR& swapchain)
    {
        m_Device.destroySwapchainKHR(swapchain);
    }

    void Device::DestroyDescriptorPool(const vk::DescriptorPool& pool)
    {
        m_Device.destroyDescriptorPool(pool);
    }

    void Device::DestroyDescriptorSetLayout(const vk::DescriptorSetLayout& layout)
    {
        m_Device.destroyDescriptorSetLayout(layout);
    }

    void Device::DestroyShaderModule(const vk::ShaderModule& module)
    {
        m_Device.destroyShaderModule(module);
    }

    void Device::DestroyCommandPool(const vk::CommandPool& commandPool)
    {
        m_Device.destroyCommandPool(commandPool);
    }

    vk::Device& Device::operator*()
    {
        return m_Device;
    }

    std::shared_ptr<Swapchain> Device::GetSwapchain() const
    {
        return m_Swapchain;
    }

    std::vector<vk::Image> Device::GetSwapchainImages(const vk::SwapchainKHR& swapchain)
    {
        return m_Device.getSwapchainImagesKHR(swapchain);
    }

    vk::Queue Device::GetQueue(const uint32_t queueFamilyIndex, const uint32_t queueIndex) const
    {
        vk::Queue queue;
        m_Device.getQueue(queueFamilyIndex, queueIndex, &queue);

        return queue;
    }

    vk::Queue Device::GetGraphicsQueue() const
    {
        return m_GraphicsQueue;
    }

    vk::Queue Device::GetTransferQueue() const
    {
        return m_TransferQueue;
    }

    vk::Queue Device::GetComputeQueue() const
    {
        return m_ComputeQueue;
    }

    vk::Queue Device::GetPresentQueue() const
    {
        return m_PresentQueue;
    }

    // ---------- ALLOCATIONS ---------------

    std::vector<vk::DescriptorSet> Device::AllocateDescriptorSets(const vk::DescriptorSetAllocateInfo& allocInfo)
    {
        return m_Device.allocateDescriptorSets(allocInfo);
    }

    std::vector<vk::CommandBuffer> Device::AllocateCommandBuffers(const vk::CommandBufferAllocateInfo& allocInfo)
    {
        return m_Device.allocateCommandBuffers(allocInfo);
    }

    // --------- WATING OPTS ----------------

    vk::Result Device::WaitForFences(const vk::ArrayProxy<vk::Fence>& fences, const bool waitForAll, uint64_t timeout)
    {
        return m_Device.waitForFences(fences, waitForAll, timeout);
    }

    void Device::WaitIdle()
    {
        return m_Device.waitIdle();
    }

    void Device::ResetFences(const vk::ArrayProxy<vk::Fence>& fences)
    {
        m_Device.resetFences(fences);
    }

    void Device::FreeCommandBuffers(const vk::CommandPool& commandPool,
                                    const std::vector<vk::CommandBuffer>& commandBuffers) const
    {
        m_Device.freeCommandBuffers(commandPool, commandBuffers);
    }

    void Device::FreeCommandBuffer(const vk::CommandPool& commandPool, const vk::CommandBuffer& commandBuffer) const
    {
        m_Device.freeCommandBuffers(commandPool, commandBuffer);
    }

    void Device::ResetDescriptorPool(const vk::DescriptorPool& pool, const vk::DescriptorPoolResetFlags& resetFlags)
    {
        m_Device.resetDescriptorPool(pool, resetFlags);
    }

    void Device::UpdateDescriptorSets(const std::vector<vk::WriteDescriptorSet>& writes)
    {
        m_Device.updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
    }

    void Device::UpdateDescriptorSets(const std::vector<vk::WriteDescriptorSet>& writes,
                                      std::vector<vk::CopyDescriptorSet>& copies)
    {
        m_Device.updateDescriptorSets(writes, copies);
    }

    vk::CommandBuffer Device::BeginSingleTimeCommands(vk::CommandPool& outCmdPool) const
    {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
        poolInfo.queueFamilyIndex = m_QueueFamilyIndices.m_GraphicsFamily.value();

        vk::CommandBuffer commandBuffer;

        TRY_CATCH_BEGIN()

        outCmdPool = m_Device.createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo{outCmdPool, vk::CommandBufferLevel::ePrimary, 1};

        commandBuffer = m_Device.allocateCommandBuffers(allocInfo)[0];

        TRY_CATCH_END()

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void Device::EndSingleTimeCommands(const vk::CommandBuffer& cmdBuffer, const vk::CommandPool& cmdPool) const
    {
        cmdBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(cmdBuffer);

        m_GraphicsQueue.submit(submitInfo);
        m_GraphicsQueue.waitIdle();

        // Don't forget to free the Command buffer after it is done copying!
        m_Device.freeCommandBuffers(cmdPool, cmdBuffer);
        m_Device.destroyCommandPool(cmdPool);
    }

    void Device::InitQueues(const QueueFamilyIndices& indices)
    {
        if (!indices.IsComplete())
        {
            LOG(Vulkan, Fatal,
                "Couldn't initialize vk::queue objects! Either graphics queue family or presentation "
                "queue family is not present!")

            throw std::runtime_error(
                "Couldn't initialize vk::queue objects! Either graphics queue family or presentation "
                "queue family is not present!");
        }

        m_GraphicsQueue = GetQueue(indices.m_GraphicsFamily.value(), 0);
        LOG(Vulkan, Verbose, "Created the graphics queue")

        m_PresentQueue = GetQueue(indices.m_PresentFamily.value(), 0);
        LOG(Vulkan, Verbose, "Created the presentation queue")

        if (indices.m_TransferFamily.has_value())
        {
            m_TransferQueue = GetQueue(indices.m_TransferFamily.value(), 0);
            LOG(Vulkan, Verbose, "Created the Transfer Queue")
        }
        else
        {
            LOG(Vulkan, Warning, "Couldn't find the transfer family queue! Skipped creation of the Transfer queue.")
        }

        if (indices.m_ComputeFamily.has_value())
        {
            m_ComputeQueue = GetQueue(indices.m_ComputeFamily.value(), 0);
            LOG(Vulkan, Verbose, "Created the Compute Queue")
        }
        else
        {
            LOG(Vulkan, Warning, "Couldn't find the transfer family queue! Skipped creation of the compute queue.")
        }

        m_QueueFamilyIndices = indices;

        LOG(Vulkan, Verbose, "Queues done initializing...")
    }

    vk::ResultValue<uint32_t> Device::AcquireNextImageKHR(const vk::Semaphore& semaphore, const vk::Fence& fence,
                                                          const uint64_t timeout)
    {
        return m_Device.acquireNextImageKHR(m_Swapchain->GetVkSwapchain(), timeout, semaphore, fence);
    }

} // namespace VkCore
