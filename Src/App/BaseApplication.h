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
#include "vk_mem_alloc.h"

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
        void Shutdown();

      private:
        void CreateWindow();
        void CreateInstance();
        void CreateDevices();
        void CreateServices();
        
        void CreatePipeline();


        std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        vk::DebugUtilsMessengerEXT m_DebugMessenger;
        vk::Instance m_Instance;
        RenderPass m_RenderPass;

        PhysicalDevice* m_PhysicalDevice;
        Device* m_Device;

        DescriptorBuilder m_DescriptorBuilder;

        std::unique_ptr<Window> m_Window;
        uint32_t m_WinWidth, m_WinHeight;
        std::string m_Title;
    };

} // namespace VkCore
