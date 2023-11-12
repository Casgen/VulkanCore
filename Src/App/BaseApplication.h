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
    class BaseApplication
    {
      public:
        BaseApplication(const uint32_t width, const uint32_t height, const std::string& title);

        BaseApplication* Get()
        {
            return s_Instance;
        };

        virtual void PreInitVulkan() = 0;
        void InitVulkan();
        virtual void PostInitVulkan() = 0;

        void Run();
        void Loop();
        virtual void DrawFrame() = 0;
        virtual void RecordCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex) = 0;
        virtual void Shutdown() = 0;

        virtual void OnEvent(Event& event);
        virtual bool OnMousePress(MouseButtonEvent& event) { return false; }
        virtual bool OnMouseMoved(MouseMovedEvent& event) { return false; }
        virtual bool OnMouseScrolled(MouseScrolledEvent& event) { return false; }
        virtual bool OnMouseRelease(MouseButtonEvent& event) { return false; }
        virtual bool OnKeyPressed(KeyPressedEvent& event) { return false; }
        virtual bool OnKeyReleased(KeyReleasedEvent& event) { return false; }
        virtual bool OnWindowResize(WindowResizedEvent& event) { return false; }

        void CreateWindow();
        void CreateInstance();
        void CreateDevices();
        void CreateServices();

      protected:
        std::vector<const char*> m_DeviceExtensions = {VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                                       VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        MouseState m_MouseState;

        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::Instance m_Instance;
        RenderPass m_RenderPass;

        PhysicalDevice m_PhysicalDevice;
        Device m_Device;

        uint32_t m_CurrentFrame = 0;

        std::unique_ptr<Window> m_Window;
        uint32_t m_WinWidth, m_WinHeight;
        std::string m_Title;

        VertexAttributeBuilder m_AttributeBuilder;
        GraphicsPipelineBuilder m_PipelineBuilder;

        std::vector<vk::Framebuffer> m_SwapchainFramebuffers;

        DescriptorBuilder m_DescriptorBuilder;

        bool m_Running;
        static inline BaseApplication* s_Instance = nullptr;

      private:
        void PreShutdown();
        void PostShutdown();
    };

} // namespace VkCore
