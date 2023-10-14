#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>

#include "../Vk/Model/Descriptors/DescriptorBuilder.h"
#include "../Vk/Model/Devices/Device.h"
#include "../Platform/Window.h"
#include "../Vk/Model/RenderPass.h"
#include "../Vk/Model/GraphicsPipeline/GraphicsPipelineBuilder.h"

#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"

namespace VkCore
{
    class BaseApplication
    {
      public:
        BaseApplication(const uint32_t width, const uint32_t height, const std::string& title);
        ~BaseApplication();

        void PreInitVulkan();
        void InitVulkan();
        void PostInitVulkan();

        void Run();
        void Loop();
        void DrawFrame();
        void RecordCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex);
        void Shutdown();


      private:
        void CreateWindow();
        void CreateInstance();
        void CreateDevices();
        void CreateServices();
        
        void CreateVertexBuffer();
        void CreatePipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSyncObjects();


        std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::Instance m_Instance;
        RenderPass m_RenderPass;

        PhysicalDevice* m_PhysicalDevice;
        Device* m_Device;

        uint32_t m_CurrentFrame = 0;

        DescriptorBuilder m_DescriptorBuilder;

        std::unique_ptr<Window> m_Window;
        uint32_t m_WinWidth, m_WinHeight;
        std::string m_Title;

        VertexAttributeBuilder m_AttributeBuilder;
        GraphicsPipelineBuilder m_PipelineBuilder;

        Buffer m_VertexBuffer;
        VkBuffer m_VkBuffer = VK_NULL_HANDLE;
        VmaAllocationInfo m_VertexAllocInfo{};
        VmaAllocation m_VertexAlloc{};

        vk::Pipeline m_Pipeline;

        std::vector<vk::Framebuffer> m_SwapchainFramebuffers;

        // Commands
        vk::CommandPool m_CommandPool;
        std::vector<vk::CommandBuffer> m_CommandBuffers;

        // Sync Objects
        std::vector<vk::Fence> m_InFlightFences;
        std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::Semaphore> m_ImageAvailableSemaphores;


    };

} // namespace VkCore
