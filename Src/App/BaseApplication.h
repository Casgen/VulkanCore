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
#include "../Model/Camera.h"
#include "../Event/Event.h"
#include "../Event/KeyEvent.h"
#include "../Event/WindowEvent.h"
#include "../Event/MouseEvent.h"

#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"
#include "../Model/MatrixBuffer.h"

namespace VkCore
{
    class Application
    {
      public:
        Application(const uint32_t width, const uint32_t height, const std::string& title);
        virtual ~Application();

        Application* Get() { return s_Instance; };

        void PreInitVulkan();
        void InitVulkan();
        void PostInitVulkan();

        void Run();
        void Loop();
        void DrawFrame();
        void RecordCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex);
        void Shutdown();

        void OnEvent(Event& event);
        bool OnMousePress(MouseButtonEvent& event);
        bool OnMouseMoved(MouseMovedEvent& event);
        bool OnMouseScrolled(MouseScrolledEvent& event);
        bool OnMouseRelease(MouseButtonEvent& event);
        bool OnKeyPressed(KeyPressedEvent& event);
        bool OnKeyReleased(KeyReleasedEvent& event);
        bool OnWindowResize(WindowResizedEvent& event);

      private:
        void CreateWindow();
        void CreateInstance();
        void CreateDevices();
        void CreateServices();

        void CreateBuffers();
        void CreateDescriptorSets();
        void CreatePipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSyncObjects();

        std::vector<const char*> m_DeviceExtensions = {VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                                       VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        MouseState m_MouseState;

        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::Instance m_Instance;
        RenderPass m_RenderPass;

        PhysicalDevice* m_PhysicalDevice;
        Device m_Device;

        uint32_t m_CurrentFrame = 0;

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
        vk::PipelineLayout m_PipelineLayout;

        std::vector<vk::Framebuffer> m_SwapchainFramebuffers;

        // Commands
        vk::CommandPool m_CommandPool;
        std::vector<vk::CommandBuffer> m_CommandBuffers;

        // Sync Objects
        std::vector<vk::Fence> m_InFlightFences;
        std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::Semaphore> m_ImageAvailableSemaphores;

        Camera m_Camera;
        std::vector<Buffer> m_MatBuffers;

        DescriptorBuilder m_DescriptorBuilder;
        std::vector<vk::DescriptorSet> m_DescriptorSets;
        vk::DescriptorSetLayout m_DescriptorSetLayout;

        bool m_Running;
        static inline Application* s_Instance = nullptr;
    };

} // namespace VkCore
