#include <cstdint>
#include <memory>

#include "BaseApplication.h"
#include "GLFW/glfw3.h"
#include "../Log/Log.h"
#include "../Vk/SwapchainRenderPass.h"
#include "../Vk/Utils.h"
#include "../Vk/Services/Allocator/VmaAllocatorService.h"
#include "../Vk/Services/ServiceLocator.h"
#include "../Vk/Devices/DeviceManager.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"

namespace VkCore
{

    BaseApplication::BaseApplication(const uint32_t width, const uint32_t height, const std::string& title)
        : m_Title(title), m_InitWinWidth(width), m_InitWinHeight(height)
    {
        s_Instance = this;

        Logger::SetSeverityFilter(ESeverity::Verbose);

        DeviceManager::AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        DeviceManager::AddDeviceExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
    }

    void BaseApplication::InitVulkan()
    {

        PreInitVulkan();

        // Init Window. The window has to be created before the instance in order to obtain the required extensions
        // for the VkInstance!
        VkCore::WindowProps windowProps{m_Title, m_InitWinWidth, m_InitWinHeight};

        m_Window = std::make_unique<VkCore::Window>(m_Instance, windowProps);
        m_Window->SetEventCallback(std::bind(&BaseApplication::OnEvent, this, std::placeholders::_1));

        m_Running = true;

        // Init Instance
        std::vector<const char*> requiredExtensions(Window::GetRequiredInstanceExtensions()), layers;

#ifdef DEBUG
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        m_Instance = VkCore::Utils::CreateInstance(m_Title, VK_API_VERSION_1_3, requiredExtensions, layers, true);

#ifdef DEBUG
        // Debug messenger has to be created after creating the VkInstance!
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(VkCore::Utils::PopulateDebugMessengerCreateInfo());
#endif

        m_Window->InitSurface(m_Instance);

        // Init Devices
        DeviceManager::Initialize(m_Instance, m_Window->GetVkSurface());

        // Init Services
        VmaAllocatorService* allocationService = new VmaAllocatorService(m_Instance);
        ServiceLocator::ProvideAllocatorService(allocationService);

        m_Swapchain = VkCore::Swapchain(m_Window->GetVkSurface(), m_Window->GetHeight(), m_Window->GetWidth());
        m_DescriptorBuilder = DescriptorBuilder(DeviceManager::GetDevice());

        m_RenderPass = VkCore::SwapchainRenderPass(m_Swapchain);
        PostInitVulkan();
    }

    void BaseApplication::Run()
    {
        InitVulkan();
        Loop();
        PreShutdown();
        Shutdown();
        PostShutdown();
    };

    void BaseApplication::Loop()
    {
        while (!m_Window->ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }
    }

    void BaseApplication::PreShutdown()
    {
        DeviceManager::GetDevice().WaitIdle();
    }

    void BaseApplication::PostShutdown()
    {
        DeviceManager::GetDevice().Destroy();
        m_Instance.destroy();
    }

    void BaseApplication::OnEvent(Event& event)
    {
        EventDispatcher dispatcher = EventDispatcher(event);

        switch (event.GetEventType())
        {
        case EventType::MouseBtnPressed:
            dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(BaseApplication::OnMousePress));
            break;
        case EventType::MouseMoved:
            dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(BaseApplication::OnMouseMoved));
            break;
        case EventType::MouseBtnReleased:
            dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(BaseApplication::OnMouseRelease));
            break;
        case EventType::KeyPressed:
            dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(BaseApplication::OnKeyPressed));
            break;
        case EventType::KeyReleased:
            dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(BaseApplication::OnKeyReleased));
            break;
        case EventType::MouseScrolled:
            dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(BaseApplication::OnMouseScrolled));
            break;
        case EventType::WindowResized:
            dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(BaseApplication::OnWindowResize));
            break;
        }
    }

} // namespace VkCore
