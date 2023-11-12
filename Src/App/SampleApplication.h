#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>

#include "../Model/Camera.h"
#include "BaseApplication.h"
#include "../Model/MatrixBuffer.h"

namespace VkCore
{
    class SampleApplication : public BaseApplication
    {
      public:
        SampleApplication(const uint32_t width, const uint32_t height, const std::string& title);

        virtual void PreInitVulkan() override {};
        virtual void PostInitVulkan() override;

        virtual void Shutdown() override;
        virtual void DrawFrame() override;

        void CreateBuffers();
        void CreateDescriptorSets();
        void CreatePipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSyncObjects();

        virtual bool OnMousePress(MouseButtonEvent& event) override;
        virtual bool OnMouseMoved(MouseMovedEvent& event) override;
        virtual bool OnMouseScrolled(MouseScrolledEvent& event) override;
        virtual bool OnMouseRelease(MouseButtonEvent& event) override;
        virtual bool OnKeyPressed(KeyPressedEvent& event) override;
        virtual bool OnKeyReleased(KeyReleasedEvent& event) override;
        virtual bool OnWindowResize(WindowResizedEvent& event) override;

        virtual void RecordCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex) override;

    private:
        vk::Pipeline m_Pipeline;
        vk::PipelineLayout m_PipelineLayout;

        // Commands
        vk::CommandPool m_CommandPool;
        std::vector<vk::CommandBuffer> m_CommandBuffers;

        // Sync Objects
        std::vector<vk::Fence> m_InFlightFences;
        std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::Semaphore> m_ImageAvailableSemaphores;

        Camera m_Camera;
        std::vector<Buffer> m_MatBuffers;

        std::vector<vk::DescriptorSet> m_DescriptorSets;
        vk::DescriptorSetLayout m_DescriptorSetLayout;

        Buffer m_VertexBuffer;
        VkBuffer m_VkBuffer = VK_NULL_HANDLE;
        VmaAllocationInfo m_VertexAllocInfo{};
        VmaAllocation m_VertexAlloc{};
    };

} // namespace VkCore
