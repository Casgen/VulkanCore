
#include <memory>
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"

#define VMA_IMPLEMENTATION
#include "BaseApplication.h"
#include "GLFW/glfw3.h"
#include "../Log/Log.h"
#include "../Vk/Model/SwapchainRenderPass.h"
#include "../Vk/Utils.h"
#include "../Vk/Model/Services/Allocator/VmaAllocatorService.h"
#include "../Vk/Model/Services/ServiceLocator.h"


namespace VkCore
{

    BaseApplication::BaseApplication(const uint32_t width, const uint32_t height, const std::string& title)
        : m_WinWidth(width), m_WinHeight(height), m_Title(title)
    {
        Logger::SetSeverityFilter(ESeverity::Verbose);
    }

    void BaseApplication::PreInitVulkan()
    {
        VkCore::WindowProps windowProps{m_Title, m_WinWidth, m_WinHeight};

        m_Window = std::make_unique<VkCore::Window>(m_Instance, windowProps);

        std::vector<const char*> requiredExtensions(m_Window->GetRequiredInstanceExtensions()), layers;

#ifdef DEBUG
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        m_Instance = VkCore::Utils::CreateInstance(m_Title, VK_API_VERSION_1_3, requiredExtensions, layers, true);

#ifdef DEBUG
        // Debug messenger has to be created after creating the VkInstance!
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(VkCore::Utils::PopulateDebugMessengerCreateInfo());
#endif

        m_Window->CreateSurface(m_Instance);

        m_PhysicalDevice = new VkCore::PhysicalDevice(m_Instance, m_Window->GetSurface(), m_DeviceExtensions);
        m_Device = new VkCore::Device(*m_PhysicalDevice, m_DeviceExtensions);

        VmaAllocatorService* allocationService = new VmaAllocatorService(*m_Device, *m_PhysicalDevice, m_Instance);
        ServiceLocator::ProvideAllocatorService(allocationService);

        m_Device->InitSwapChain(*m_PhysicalDevice, m_Window->GetSurface(), m_WinWidth, m_WinHeight);
    }

    void BaseApplication::InitVulkan()
    {
        m_RenderPass = VkCore::SwapchainRenderPass(*m_Device);

        float vertices[15] = {
            -0.5f, 0.5f,        1.f, 0.f, 0.f,
            .0f, 0.5f,          0.f, 1.f, 0.f,
            0.5f, -0.5f,        0.f, 0.f, 1.f,
        };

        Buffer vertexBuffer = Buffer(&vertices, sizeof(float) * 15, vk::BufferUsageFlagBits::eVertexBuffer);
    }

    BaseApplication::~BaseApplication()
    {
    }

    void BaseApplication::Run()
    {
        PreInitVulkan();
        InitVulkan();
        Loop();
    };

    void BaseApplication::Loop()
    {
        while (!m_Window->ShouldClose())
        {
            glfwPollEvents();
        }

        m_Instance.destroy();
    }

    void BaseApplication::CreateVmaAllocator(VkCore::Device& device, VkCore::PhysicalDevice& physicalDevice, vk::Instance& instance)
    {
        VmaAllocatorCreateInfo createInfo;

        createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        createInfo.device = *device;
        createInfo.physicalDevice = *physicalDevice;
        createInfo.instance = instance;
        
        vmaCreateAllocator(&createInfo, &m_VmaAllocator);
    }

} // namespace VkCore
