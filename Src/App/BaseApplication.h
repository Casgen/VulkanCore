#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "../Vk/Descriptors/DescriptorBuilder.h"
#include "../Vk/Devices/Device.h"
#include "../Platform/Window.h"
#include "../Vk/SwapchainRenderPass.h"
#include "../Vk/GraphicsPipeline/GraphicsPipelineBuilder.h"
#include "../Event/Event.h"
#include "../Event/KeyEvent.h"
#include "../Event/WindowEvent.h"
#include "../Event/MouseEvent.h"

#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"

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

        virtual bool OnMousePress(MouseButtonEvent& event)
        {
            return false;
        }
        virtual bool OnMouseMoved(MouseMovedEvent& event)
        {
            return false;
        }
        virtual bool OnMouseScrolled(MouseScrolledEvent& event)
        {
            return false;
        }
        virtual bool OnMouseRelease(MouseButtonEvent& event)
        {
            return false;
        }
        virtual bool OnKeyPressed(KeyPressedEvent& event)
        {
            return false;
        }
        virtual bool OnKeyReleased(KeyReleasedEvent& event)
        {
            return false;
        }
        virtual bool OnWindowResize(WindowResizedEvent& event)
        {
            return false;
        }

        void CreateWindow();
        void CreateInstance();
        void CreateDevices();
        void CreateServices();

      protected:
        MouseState m_MouseState;

        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::Instance m_Instance;
        SwapchainRenderPass m_RenderPass;

        uint32_t m_CurrentFrame = 0;
        bool m_FramebufferResized = false;

        std::unique_ptr<Window> m_Window;
        std::string m_Title;

        VertexAttributeBuilder m_AttributeBuilder;
        GraphicsPipelineBuilder m_PipelineBuilder;

        std::vector<vk::Framebuffer> m_SwapchainFramebuffers;

        DescriptorBuilder m_DescriptorBuilder;

        bool m_Running;
        static inline BaseApplication* s_Instance = nullptr;

      private:
        uint32_t m_InitWinWidth = 0;
        uint32_t m_InitWinHeight = 0;

        void PreShutdown();
        void PostShutdown();
    };

} // namespace VkCore
