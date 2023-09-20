
#include <cstdint>
#include <memory>
#include "glm/fwd.hpp"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"

#define VMA_IMPLEMENTATION
#include "BaseApplication.h"
#include "GLFW/glfw3.h"
#include "../Log/Log.h"
#include "../Vk/Model/SwapchainRenderPass.h"
#include "../Vk/Utils.h"
#include "../Vk/Model/Services/Allocator/VmaAllocatorService.h"
#include "../Vk/Model/Services/ServiceLocator.h"
#include "../FileUtils.h"
#include "../Model/Shaders/ShaderLoader.h"

namespace VkCore
{

    BaseApplication::BaseApplication(const uint32_t width, const uint32_t height, const std::string& title)
        : m_WinWidth(width), m_WinHeight(height), m_Title(title)
    {
        Logger::SetSeverityFilter(ESeverity::Verbose);
    }

    BaseApplication::~BaseApplication()
    {
    }

    void BaseApplication::PreInitVulkan()
    {
    }

    void BaseApplication::InitVulkan()
    {
        // The window has to be created before the instance in order to obtain the required extensions
        // for the VkInstance!
        CreateWindow();
        CreateInstance();

        m_Window->CreateSurface(m_Instance);

        CreateDevices();

        m_DescriptorBuilder = DescriptorBuilder(*m_Device);

        CreateServices();

        m_Device->InitSwapChain(*m_PhysicalDevice, m_Window->GetSurface(), m_WinWidth, m_WinHeight);
    }

    void BaseApplication::PostInitVulkan()
    {
        // Render Pass
        m_RenderPass = VkCore::SwapchainRenderPass(*m_Device);

        // Buffer
        float* vertices = new float[]{
            -0.5f, .5f, 1.f, 0.f, 0.f, .0f, .5f, 0.f, 1.f, 0.f, .5f, -0.5f, 0.f, 0.f, 1.f,
        };

        Buffer vertexBuffer = Buffer(&vertices, sizeof(float) * 15, vk::BufferUsageFlagBits::eVertexBuffer);

        // Decsriptor Sets

        // m_DescriptorBuilder.BindBuffer(0, vertexBuffer, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlags
        // stageFlags)

        m_AttributeBuilder.PushAttribute<float>(2).PushAttribute<float>(3);

        CreatePipeline();
    }

    void BaseApplication::CreatePipeline()
    {
        const std::vector<ShaderData> shaders = ShaderLoader::LoadClassicShaders("Res/Shaders/");

        m_Pipeline = m_PipelineBuilder.BindShaderModules(shaders)
            .BindRenderPass(m_RenderPass.GetRenderPass())
            .AddViewport(glm::uvec4(0,0,m_WinWidth, m_WinHeight))
            .BindVertexAttributes(m_AttributeBuilder)
            .SetPrimitiveAssembly(vk::PrimitiveTopology::eLineStrip)
            .Build();
               
    }

    void BaseApplication::Run()
    {
        PreInitVulkan();
        InitVulkan();
        PostInitVulkan();
        Loop();
        Shutdown();
    };

    void BaseApplication::Loop()
    {
        while (!m_Window->ShouldClose())
        {
            glfwPollEvents();
        }
    }

    void BaseApplication::Shutdown()
    {

        m_Device->Destroy();
        m_Instance.destroy();
    }

    void BaseApplication::CreateWindow()
    {
        VkCore::WindowProps windowProps{m_Title, m_WinWidth, m_WinHeight};

        m_Window = std::make_unique<VkCore::Window>(m_Instance, windowProps);
    }

    void BaseApplication::CreateInstance()
    {

        std::vector<const char*> requiredExtensions(Window::GetRequiredInstanceExtensions()), layers;

#ifdef DEBUG
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        m_Instance = VkCore::Utils::CreateInstance(m_Title, VK_API_VERSION_1_3, requiredExtensions, layers, true);

#ifdef DEBUG
        // Debug messenger has to be created after creating the VkInstance!
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(VkCore::Utils::PopulateDebugMessengerCreateInfo());
#endif
    }

    void BaseApplication::CreateDevices()
    {
        m_PhysicalDevice = new VkCore::PhysicalDevice(m_Instance, m_Window->GetSurface(), m_DeviceExtensions);
        m_Device = new VkCore::Device(*m_PhysicalDevice, m_DeviceExtensions);
    }

    void BaseApplication::CreateServices()
    {
        VmaAllocatorService* allocationService = new VmaAllocatorService(*m_Device, *m_PhysicalDevice, m_Instance);
        ServiceLocator::ProvideAllocatorService(allocationService);
    }

} // namespace VkCore
