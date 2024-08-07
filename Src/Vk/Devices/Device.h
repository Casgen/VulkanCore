#pragma once

#include <cstdint>
#include <vector>

#include "PhysicalDevice.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace VkCore
{

    class Swapchain;
    class Device
    {
      public:
        Device();

        Device(const PhysicalDevice& physicalDevice,
               const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, const bool isMeshShadingEnabled = true);

        /**
         * @brief Initializes a Swapchain Object. THIS HAS TO BE CALLED AFTER CREATING A LOGICAL DEVICE!
         * @param physicalDevice
         */
        void InitSwapChain(const PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface,
                           const uint32_t desiredWidth, const uint32_t desiredHeight);

        // ---------- CREATORS -------------
        vk::RenderPass CreateRenderPass(const vk::RenderPassCreateInfo createInfo) const;
        vk::SwapchainKHR CreateSwapchain(const vk::SwapchainCreateInfoKHR createInfo);
        vk::ImageView CreateImageView(const vk::ImageViewCreateInfo& createInfo);
        vk::DescriptorPool CreateDescriptorPool(const vk::DescriptorPoolCreateInfo& createInfo);
        vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo);
        vk::CommandPool CreateCommandPool(const vk::CommandPoolCreateInfo& createInfo);
        vk::ShaderModule CreateShaderModule(const vk::ShaderModuleCreateInfo& createInfo);
        vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& createInfo);
        vk::Framebuffer CreateFrameBuffer(const vk::FramebufferCreateInfo& createInfo);
        vk::ResultValue<std::vector<vk::Pipeline>> CreateGraphicsPipelines(
            const std::vector<vk::GraphicsPipelineCreateInfo>& createInfo);
        vk::Semaphore CreateSemaphore(const vk::SemaphoreCreateInfo& createInfo);
        vk::Fence CreateFence(const vk::FenceCreateInfo& createInfo);
        vk::QueryPool CreateQueryPool(const vk::QueryPoolCreateInfo createInfo) const;
        vk::ResultValue<vk::Pipeline> CreateComputePipeline(const vk::ComputePipelineCreateInfo createInfo,
                                                            const vk::PipelineCache cache = nullptr) const;
        vk::Sampler CreateSampler(const vk::SamplerCreateInfo createInfo) const;

        // ---------- DESTROYERS -----------

        void DestroyRenderPass(const vk::RenderPass& renderPass);
        void DestroyFrameBuffer(const vk::Framebuffer& frameBuffer);
        void DestroyFrameBuffers(const std::vector<vk::Framebuffer>& frameBuffers);
        void DestroyImageView(const vk::ImageView& imageView);
        void DestroySampler(const vk::Sampler& sampler);
        void DestroyImage(const vk::Image& image);

        void DestroyFences(const std::vector<vk::Fence>& fences);
        void DestroySemaphores(const std::vector<vk::Semaphore>& semaphores);

        void DestroyFence(const vk::Fence& fence);
        void DestroySemaphore(const vk::Semaphore& semaphore);

        void DestroySwapchain(const vk::SwapchainKHR& swapchain);
        void DestroyDescriptorPool(const vk::DescriptorPool& pool);
        void DestroyDescriptorSetLayout(const vk::DescriptorSetLayout& layout);
        void DestroyShaderModule(const vk::ShaderModule& module);
        void DestroyCommandPool(const vk::CommandPool& commandPool);
        void DestroyPipeline(const vk::Pipeline& pipeline);
        void DestroyPipelineLayout(const vk::PipelineLayout& pipelineLayout);
        void DestroyQueryPool(const vk::QueryPool& queryPool) const;
        void FreeDescriptorSet(const vk::DescriptorPool descPool, const vk::DescriptorSet& descSet);
        void Destroy();

        // ------------ GETTERS ------------

        vk::Device& operator*();
        std::vector<vk::Image> GetSwapchainImages(const vk::SwapchainKHR& swapchain);
        vk::Queue GetQueue(const uint32_t queueFamilyIndex, const uint32_t queueIndex) const;

        vk::Queue GetGraphicsQueue() const;
        vk::Queue GetTransferQueue() const;
        vk::Queue GetComputeQueue() const;
        vk::Queue GetPresentQueue() const;

        // ------------ ALLOCATIONS --------
        std::vector<vk::DescriptorSet> AllocateDescriptorSets(const vk::DescriptorSetAllocateInfo& allocInfo);
        std::vector<vk::CommandBuffer> AllocateCommandBuffers(const vk::CommandBufferAllocateInfo& allocInfo);

        // --------- SYNC OPTS -----------

        /**
         *   Tells the device to wait for the fences (or a fence) to be signaled.
         *   @param fences - fences to be waited on upon signaling.
         *   @param waitForAll - Indicates that for all the fences should be waited on for their signaling
         *   @param timeout - The maximum time for which we should wait for the fences to be signaled. If the timeout is
         * reached, the function will fail.
         */
        vk::Result WaitForFences(const vk::ArrayProxy<vk::Fence>& fences, const bool waitForAll,
                                 uint64_t timeout = UINT64_MAX);

        /**
         * Waits for any operations to be completed on the whole device (GPU).
         */
        void WaitIdle();

        void ResetFences(const vk::ArrayProxy<vk::Fence>& fences);

        // -------- FREEING METHODS --------

        void FreeCommandBuffers(const vk::CommandPool& commandPool,
                                const std::vector<vk::CommandBuffer>& commandBuffers) const;
        void FreeCommandBuffer(const vk::CommandPool& commandPool, const vk::CommandBuffer& commandBuffer) const;

        void ResetDescriptorPool(const vk::DescriptorPool& pool, const vk::DescriptorPoolResetFlags& resetFlags =
                                                                     vk::Flags<vk::DescriptorPoolResetFlagBits>(0));

        void UpdateDescriptorSets(const std::vector<vk::WriteDescriptorSet>& writes);
        void UpdateDescriptorSets(const std::vector<vk::WriteDescriptorSet>& writes,
                                  std::vector<vk::CopyDescriptorSet>& copies);

        vk::CommandBuffer BeginSingleTimeCommands(vk::CommandPool& outCmdPool) const;
        void EndSingleTimeCommands(const vk::CommandBuffer& cmdBuffer, const vk::CommandPool& cmdPool) const;

        /**
         * Initializes all the queues which can be made (Inits the vk::Queue objects)
         * @param indices - Family queue indices (can be obtained from the VkCore::PhysicalDevice)
         */
        void InitQueues(const QueueFamilyIndices& indices);

      private:
        QueueFamilyIndices m_QueueFamilyIndices;
        vk::Device m_Device;

        vk::Queue m_GraphicsQueue;
        vk::Queue m_TransferQueue;
        vk::Queue m_ComputeQueue;
        vk::Queue m_PresentQueue;
    };

} // namespace VkCore
